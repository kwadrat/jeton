#include "fsmar.h"

#if AKTYWNY_FSMAR
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

smar_Class::smar_Class(void)
{
 RodzajWej = 'M';
 RodzajWyj = 'M';
 ShadowState = STATE_ON;
 LewyMiejsce = 0; /* Na razie nie ma danych w lewym, ani w prawym procesie */
 LewyZajete = 0;
 PrawyMiejsce = 0;
 PrawyZajete = 0;
}

void smar_Class::LocalDestructor(void)
{
 /* Tutaj nie może być dalszego zwalniania, bo tym się będzie zajmował osobny
  * wątek */
#if 0
 if(NextProcess != NULL)
 {
  NextProcess->LocalDestructor();
  delete NextProcess;
  NextProcess = NULL;
 }
#endif
 DostMiejsce.LocalDestructor();
 DostDane.LocalDestructor();
 if(Dane != NULL)
 {
  free(Dane);
  Dane = NULL;
 }
}

/* Funkcja dokonuje przydziału pamięci i inicjalizuje odpowiednie bufory. W
 * buforze pamięci najpierw jest tablica wartości typu "int" z opisem stanu
 * poszczególnych buforów, a potem są ciągi bajtów - dane poszczególnych
 * buforów.
 * Wartość zwrotna: 1 - OK, 0 - błąd */
int smar_Class::AdvancedInit(int ile, int rozmiar)
{
 int status;
 int IlePamieci; /* Liczba bajtów pamięci przydzielona w buforze */
 int i;
 int * wsk; /* Wskaźnik pomocniczy do inicjalizacji pamięci */

 /* Tutaj musimy przydzielić bufor w pamięci oraz semafory */
 status = 0;
 assert(ile > 0);
 assert(rozmiar > 0);
 RozmBuf = rozmiar;
 /* W pamięci musimy przechować w pierwszym bajcie */
 LiczbaBuf = ile;
 IlePamieci = LiczbaBuf * (sizeof(int) + RozmBuf);
 Dane = (Byte *) malloc(IlePamieci);
 if(Dane != NULL)
 {
  wsk = (int *) Dane;
  for(i = 0; i < LiczbaBuf; i++)
  {
   wsk[i] = RozmBuf; /* Ustawiamy ilość wolnego miejsca w buforach */
  }
  if(DostDane.UstawWartosc(0, LiczbaBuf))
  {
   if(DostMiejsce.UstawWartosc(LiczbaBuf, LiczbaBuf))
   {
    status = 1;
   }
   else
   {
    SygErrorParm("Nie udało się ustawić wartości %d dla DostMiejsce", LiczbaBuf);
   }
  }
  else
  {
   SygErrorParm("Nie udało się ustawić wartości %d dla DostDane", 0);
  }
 }
 else
 {
  SygErrorParm("Nie udało się przydzielić %d bajtów pamięci", IlePamieci);
 }
 return status;
}

int smar_Class::Init(int argc, char *argv[])
{
 int ile;
 int rozmiar;
 int status;
 int nr; /* Numer aktualnie przetwarzanego parametru */
 status = RESULT_OFF;
 PokazujWykorzystanie = 0;
 IleJestWykorzystane = 0;
 /* Chcemy sprawdzić wartość następnego parametru, dlatego wymagamy, aby była
  * podana co najmniej nazwa funkcji "smar" i jeden parametr */
 if(argc >= 2)
 {
  nr = 1; /* Zaczynamy analizę od pierwszego argumentu */
  /* Możliwość podania opcji "-m" do raportowania na końcu liczby
   * wykorzystanych buforów */
  if(! strcmp(argv[nr], "-m"))
  {
   nr ++;
   PokazujWykorzystanie = 1;
  }
  if(argc >= nr + 2)
  ile = atoi(argv[nr]);
  if(ile > 0)
  {
   rozmiar = atoi(argv[nr + 1]);
   if(rozmiar > 0)
   {
    if(AdvancedInit(ile, rozmiar))
    {
     status = nr + 2;
    }
    else
    {
     SygError("Nie powiodła się zaawansowana inicjalizacja");
    }
   }
   else
   {
    SygErrorParm("Nie udało się wczytać wielkości bufora z napisu %s", argv[2]);
   }
  }
  else
  {
   SygErrorParm("Nie udało się wczytać liczby buforów z napisu %s", argv[1]);
  }
 }
 else
 {
  SygError("Brakuje parametrów");
 }
 return status;
}

int smar_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int status;

 assert(ile == KIER_PROSTO || ile == KIER_WSTECZ);
 assert(poczatek == NULL);
 status = RESULT_OFF;
 if(State != STATE_OFF)
 {
  switch(kmn)
  {
   case SAND_GENERAL:
   {
    status = Obsluga_SAND_GENERAL(kmn, poczatek, ile);
    break;
   }
   case SAND_OFF:
   {
    status = Obsluga_SAND_OFF(kmn, poczatek, ile);
    break;
   }
   default:
   {
    SygErrorParm("Nieznany typ komunikatu %d", kmn);
    break;
   }
  }
 }
 return status;
}

/* Ta funkcja może być wywoływana tylko przez pierwszy proces dla bufora,
drugi proces nie może zmieniać statusu tego bufora. */
void smar_Class::ZmienStan(int nowy, int kier)
{
 assert(nowy == STATE_OFF || nowy == STATE_EOF);
 assert(kier == KIER_PROSTO || kier == KIER_WSTECZ || kier == KIER_INNY);
 if(kier == KIER_PROSTO)
 {
  /* Jesteśmy w prawym fragmencie procesu */
  ShadowState = nowy;
  if(ShadowState == STATE_OFF)
  {
   NextProcess->Work(SAND_OFF, NULL, KIER_PROSTO);
  }
 }
 else
 {
  if(kier == KIER_WSTECZ)
  {
   /* Jesteśmy w lewym fragmencie procesu */
   State = nowy;
  }
  else
  {
   /* Nie powinniśmy obsługiwać takiego komunikatu */
   SygErrorParm("Dostaliśmy błędny kier = %d", kier);
  }
 }
}

/* Wykonywane w osobnym wątku. Kolejne wątki są tworzone od lewej do prawej
 * i od razu są uaktywniane. Może dojść do sytuacji, gdy nie wszystkie procesy
 * zostaną poprawnie utworzone i wtedy trzeba usunąć taką połowicznie utworzoną
 * strukturę. Wartość zwrotna - na razie zwraca cokolwiek (qaz), trzeba się
 * zastanowić nad znaczeniem zwracanej wartości. */
int smar_Class::FunkcjaWatka(void)
{
 int status;

 /* Ta wartość "KIER_WSTECZ" sygnalizuje specjalne uruchomienie w nowym wątku */
 status = Work(SAND_GENERAL, NULL, KIER_WSTECZ); /* Wykonywane dla bufora "this" */
 return status;
}

/* Funkcja pomocnicza do uruchamiania w osobnym wątku obsługi elementu
 * łańcucha, po to, aby rozszczepiany wątek startował w zwykłej funkcji, a nie
 * w metodzie danego obiektu. */

#ifdef PLATFORM_LINUX
void * xFunkcjaWatka(void * wsk)
{
 smar_Class * w;
 w = (smar_Class *) wsk;
 w->FunkcjaWatka();
 return (void *) NULL;
}
#endif

#ifdef PLATFORM_WIN
DWORD WINAPI xFunkcjaWatka(LPVOID wsk)
{
 smar_Class * w;
 w = (smar_Class *) (*((DWORD *)wsk));
 w->FunkcjaWatka();
 return (DWORD) NULL;
}
#endif

/* Wykonuje rzeczywiste odszczepienie wątku. Wartość zwrotna: 1 - OK,
0 - błąd */
int smar_Class::DodajWatek(void)
{
 int status;
#ifdef PLATFORM_LINUX
 int s;
#endif

 status = 0;
#ifdef PLATFORM_LINUX
 s = pthread_create( & thread, NULL, xFunkcjaWatka, (void *) this);
 if(s == 0)
 {
  status = 1;
 }
 else
 {
  SygErrorParm("Błąd w czasie tworzenia nowego wątku: %d", s);
 }
#endif

#ifdef PLATFORM_WIN
 dwThreadParam = (DWORD) this;
 hThread = CreateThread(
                        NULL, /* atrybuty */
                        0, /* rozmiar stosu */
                        xFunkcjaWatka,
                        & dwThreadParam, /* parametr wywołania */
                        0,
                        & dwThreadId); /* Ignorowany id wątka */
 if(hThread != NULL)
 {
  status = 1; /* Udało się utworzyć dodatkowy wątek */
 }
 else
 {
  SygErrorParm("Błąd w czasie tworzenia nowego wątku: %d", GetLastError());
 }
#endif
 return status;
}

/* Funkcja czeka na wątek, który powinien się zakończyć */
void smar_Class::PoczekajNaWatek(void)
{
#ifdef PLATFORM_LINUX
 int status;

 status = pthread_join(thread, NULL);
 if(status != 0)
 {
  SygErrorParm("Funkcja pthread_join zwróciła %d", status);
 }
#endif
#ifdef PLATFORM_WIN
 WaitForSingleObject(hThread, INFINITE);
 CloseHandle(hThread);
#endif

}

/* Tworzenie nowych wątków dla obsługi systemu i budzenie ich.
Wartość zwrotna: 1 - OK (udało się pomyślnie utworzyć nowe wątki),
0 - błąd (możemy kończyć pracę systemu, bo się nie udał przydział wątków */
int smar_Class::Rozszczepianie(void)
{
 int status;
 status = 0; /* Na razie zakładamy, że się nie udało nic rozszczepić */
 if(DodajWatek())
 {
  status = 1;
  if(NextProcess != NULL)
  {
   status = NextProcess->Rozszczepianie();
  }
 }
 return status;
}

/* Parametr "ile=KIER_PROSTO" - wykonujemy pierwszy wątek nad buforem,
 * "ile=KIER_WSTECZ" - wykonujemy drugi wątek nad buforem. */
int smar_Class::Obsluga_SAND_GENERAL(int kmn, Byte * poczatek, int ile)
{
 int status;
 status = RESULT_OFF;
 if(ile == KIER_WSTECZ)
 {
  status = Obsluga_SAND_GENERAL_Dla_Prawego();
 }
 else
 {
  assert(ile == KIER_PROSTO);
  status = Obsluga_SAND_GENERAL_Dla_Lewego();
 }
 return status;
}

/* Parametr "ile=0" - wykonujemy pierwszy wątek nad buforem, "ile=1" -
 * wykonujemy drugi wątek nad buforem. */
int smar_Class::Obsluga_SAND_OFF(int kmn, Byte * poczatek, int ile)
{
 assert(ile == KIER_PROSTO);
 /* Trzeba poinformować drugi proces, że są problemy */
 AktLewyBuf = DostMiejsce.SemP();
 assert(AktLewyBuf >= 0 && AktLewyBuf < LiczbaBuf);
 F_SMAR_STATUS(AktLewyBuf) = RESULT_OFF;
 DostDane.SemV();
 return RESULT_OFF;
}

/* Proces zapisujący dane do wspólnego bufora. Pracujemy do momentu, gdy będzie
 * komunikat RESULT_OFF z prawej strony lub w odpowiedzi na SAND_DR otrzymamy
 * RESULT_EOF lub RESULT_OFF. */
int smar_Class::Obsluga_SAND_GENERAL_Dla_Lewego(void)
{
 int pracuj; /* Masz się kręcić w kółko */
 int s;
 int pobieraj_dane; /* Masz pobierać dane od lewego */
 int wyslij_buf; /* Masz bufor do wysłania do prawego */
 int dodatkowy_eof; /* Będzie dodatkowy pakiet na koniec */
 int status;

 pracuj = 1; /* Na początku pracujemy w kółko */
 pobieraj_dane = 1; /* Na początku żądamy danych od lewego sąsiada */
 /* Wysyłamy dopiero, jak mamy zebrane dane lub kiedy będziemy chcieli wysłać
  * specjalny komunikat informacyjny */
 wyslij_buf = 0; /* Jeszcze nie mamy bufora z danymi gotowymi do wysłania */
 dodatkowy_eof = 0; /* Na razie nie wiemy nic o potrzebie dodatkowego pakietu */
 status = RESULT_OFF;
 if(State == STATE_ON)
 {
  while(pracuj)
  {
   /* Najpierw prosimy o przydzielenie bufora, jeśli jest potrzebny */
   if(LewyMiejsce == 0)
   {
    AktLewyBuf = DostMiejsce.SemP();
    assert(AktLewyBuf >= 0 && AktLewyBuf < LiczbaBuf);
    s = F_SMAR_STATUS(AktLewyBuf);
    if(s == RESULT_OFF)
    {
     ZmienStan(STATE_OFF, KIER_WSTECZ); /* To nie woła łańcucha wstecz */
     pracuj = 0;
     pobieraj_dane = 0; /* Nie chcemy więcej danych */
    }
    else
    {
     /* Zakładamy niezerową ilość miejsca w buforze */
     assert(s > 0);
     LewyMiejsce = s;
     LewyZajete = 0;
    }
   }
   if(dodatkowy_eof)
   {
    pracuj = 0;
    ZmienStan(STATE_EOF, KIER_WSTECZ);
    F_SMAR_STATUS(AktLewyBuf) = RESULT_EOF; /* Nie będzie więcej danych */
    wyslij_buf = 1; /* Wyślij ten komunikat o końcu pracy */
   }
   if(pobieraj_dane)
   {
    assert(LewyMiejsce - LewyZajete > 0);
    /* Mamy miejsce, żądamy teraz danych */
    s = PrevProcess->Work(SAND_DR, F_SMAR_BUF_START(AktLewyBuf) + LewyZajete,
                         LewyMiejsce - LewyZajete);
    if(s == RESULT_OFF)
    {
     /* Poniższa linia nie informuje sąsiada z prawej, a tylko zmienia status
      * aktualnego procesu */
     ZmienStan(STATE_OFF, KIER_PROSTO);
     F_SMAR_STATUS(AktLewyBuf) = RESULT_OFF; /* Nie będzie więcej danych */
     wyslij_buf = 1; /* Wyślij ten komunikat o końcu pracy */
     pracuj = 0;
    }
    else
    {
     if(s == RESULT_EOF)
     {
      wyslij_buf = 1;
      pobieraj_dane = 0; /* Już nie można żądać danych następnym razem */
      /* qaz - poniżej nie wiadomo, czy ma być KIER_PROSTO, czy KIER_WSTECZ */
      ZmienStan(STATE_EOF, KIER_PROSTO); /* Kończymy dane, nie proś o dane */
      status = RESULT_EOF;
      if(LewyZajete > 0)
      {
       F_SMAR_STATUS(AktLewyBuf) = LewyZajete;
       dodatkowy_eof = 1;
      }
      else
      {
       F_SMAR_STATUS(AktLewyBuf) = RESULT_EOF;
       ZmienStan(STATE_EOF, KIER_WSTECZ);
       pracuj = 0; /* To będzie nasza ostatnia transmisja */
      }
     }
     else
     {
      if(s == RESULT_AGAIN)
      {
       pracuj = 0; /* Kończymy na razie pętlę */
       status = RESULT_AGAIN;
      }
      else
      {
       /* Mamy odebrać pobrane dane */
       assert(s > 0);
       LewyZajete += s;
       assert(LewyZajete <= LewyMiejsce);
       if(LewyZajete == LewyMiejsce)
       {
        F_SMAR_STATUS(AktLewyBuf) = LewyZajete;
        wyslij_buf = 1;
       }
      }
     }
    }
   }
   if(wyslij_buf)
   {
    wyslij_buf = 0;
    DostDane.SemV();
    LewyMiejsce = 0;
   }
  }
  if(State != STATE_ON)
  {
   /* Jesteśmy tutaj ostatni raz, więc czekamy na pozostały wątek */
   PoczekajNaWatek();
   if(PokazujWykorzystanie)
   {
    printf("Liczba wykorzystanych buforów: %d\n",
           DostMiejsce.IleWykorzystano());
   }
  }
 }
 return status;
}

/* Proces pobierający dane ze wspólnego bufora */
int smar_Class::Obsluga_SAND_GENERAL_Dla_Prawego(void)
{
 int status;
 int s;
 int pracuj;
 int zwolnij_buf;
 int wysylaj_dane;
 int result;

 status = RESULT_OFF;
 /* Wyjście z pętli przez zmianę "pracuj" oznacza koniec pracy dla wszystkich
  * elementów do końca łańcucha */
 pracuj = 1;
 zwolnij_buf = 1;
 wysylaj_dane = 1;
 while(pracuj)
 {
  if(PrawyMiejsce == 0)
  {
   AktPrawyBuf = DostDane.SemP();
   assert(AktPrawyBuf >= 0 && AktPrawyBuf < LiczbaBuf);
   s = F_SMAR_STATUS(AktPrawyBuf);
   if(s == RESULT_OFF)
   {
    ZmienStan(STATE_OFF, KIER_PROSTO); /* Lawina wywołań */
    /* Tu nie zwalniamy już bufora */
    zwolnij_buf = 0;
    wysylaj_dane = 0;
    pracuj = 0;
   }
   else
   {
    if(s == RESULT_EOF)
    {
     ZmienStan(STATE_EOF, KIER_PROSTO);
     wysylaj_dane = 0; /* Już nic nie będziemy pompować w potok */
     status = NextProcess->Work(SAND_EOF, NULL, KIER_PROSTO);
     pracuj = 0;
    }
    else
    {
     assert(s > 0);
     PrawyZajete = s;
    }
   }
  }
  if(wysylaj_dane)
  {
   PrawyPobrane = 0;
   result = RESULT_OFF;
   while(PrawyPobrane < PrawyZajete)
   {
    s = NextProcess->Work(SAND_SR, F_SMAR_BUF_START(AktPrawyBuf) + PrawyPobrane,
                          PrawyZajete - PrawyPobrane);
    if(s == RESULT_OFF)
    {
     pracuj = 0;
     break; /* Koniec pętli */
    }
    else
    {
     assert(s > 0);
     PrawyPobrane += s;
    }
   }
   if(pracuj) /* Jeszcze pracujemy, więc zwracamy ilość pobranych danych */
   {
    assert(PrawyPobrane == PrawyZajete);
    result = RozmBuf;
   }
   F_SMAR_STATUS(AktPrawyBuf) = result;
  }
  if(zwolnij_buf)
  {
   DostMiejsce.SemV();
   PrawyMiejsce = 0;
  }
 }
 if(NextProcess != NULL)
 {
  NextProcess->LocalDestructor();
  delete NextProcess;
  NextProcess = NULL;
 }
 else
 {
  SygError("Brak następnego procesu");
 }

 return status;
}

#endif
