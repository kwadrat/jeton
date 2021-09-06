#include "fkopier.h"
#include "typy.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

kopier_Class::kopier_Class(void)
{
 RodzajWej = 'M';
 RodzajWyj = 'M';
 BufSize = 0;
 BufPtr = NULL;
 BufDelayD = 0; /* Na początku nie opóźniamy transmisji */
 BufDanych = 0; /* Na początku nie ma żadnych danych w buforze */
}

void kopier_Class::LocalDestructor(void)
{
 if(NextProcess != NULL)
 {
  NextProcess->LocalDestructor();
  delete NextProcess;
  NextProcess = NULL;
 }
 if(BufPtr != NULL)
 {
  free(BufPtr);
  BufPtr = NULL;
  BufSize = 0;
  BufDanych = 0;
 }
}

/* Jedynym parametrem jest liczba bajtów w buforze */
int kopier_Class::Init(int argc, char *argv[])
{
 int ile;
 int status;
 int nr; /* Numer przetwarzanego argumentu */
 int poprawnie;

 poprawnie = 1; /* Na razie przetwarzanie nie napotkało błędów */
 status = RESULT_OFF;
 if(argc >= 2)
 {
  nr = 1;
  if( ! strcmp(argv[nr], "-d"))
  {
   /* Potrzebujemy poprzednie dwa argumenty oraz dodatkowo opcję "-d" oraz
    * liczbę milisekund, o ile należy czekać po każdej transmisji danych */
   if(argc >= 4)
   {
    nr ++; /* Przejście na liczbę milisekund */
    BufDelayD = atoi(argv[nr]);
    if(BufDelayD > 0)
    {
     nr ++; /* Przejście na rozmiar bufora */
    }
    else
    {
     SygErrorParm("Nie udało się odczytać liczby milisekund z napisu: %s",
      argv[nr]);
     poprawnie = 0; /* Zaznaczamy, że był błąd w przetwarzaniu */
    }
   }
   else
   {
    SygErrorParm("Mamy za mało parametrów: %d", argc);
    poprawnie = 0; /* Zaznaczamy, że był błąd w przetwarzaniu */
   }
  }
  ile = atoi(argv[nr]);
  if(ile > 0)
  {
   BufSize = ile;
   BufPtr = (Byte *) malloc(BufSize * sizeof(Byte));
   if(BufPtr != NULL)
   {
    status = 1 + nr; /* "nr" to liczba przetworzonych parametrów */
   }
   else
   {
    SygErrorParm("Brakło %zu bajtów pamięci", BufSize * sizeof(Byte));
   }
  }
  else
  {
   SygErrorParm("Nie udało się wczytać wielkości bufora z napisu %s", argv[1]);
  }
 }
 else
 {
  SygError("Brakuje parametru");
 }
 return status;
}

int kopier_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int status;

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
    ZmienStan(STATE_OFF, KIER_INNY);
    break;
   }
  }
 }
 return status;
}

int kopier_Class::Obsluga_SAND_GENERAL(int kmn, Byte * poczatek, int ile)
{
 int r1, r2;
 int suma;
 int status;
 int pracuj; /* Pętla pobierania danych od poprzedniego */
 int pobieraj_dane; /* Masz pobierać dane od poprzedniego */
 int wyslij_buf; /* Należy wysłać dalej dane zgromadzone w buforze */
 int dodatkowy_eof; /* Będzie dodatkowy pakiet na koniec */
 int cykl_pobierania; /* To jest wywoływane w kółko przy pobieraniu danych */

 status = RESULT_OFF;
 assert(PrevProcess != NULL);
 assert(NextProcess != NULL);
 dodatkowy_eof = 0; /* Na razie nie chcemy żadnego końca danych */
 pobieraj_dane = 1; /* Chcemy pobierać dane od poprzedniego */
 wyslij_buf = 0; /* Jeszcze nie mam danych do wysłania */
 pracuj = 1;
 while(pracuj)
 {
  /* Dlaczego sprawdzanie końca pliku jest na samej górze głównej pętli?
   * Mogłoby być na dole, ale nie jest, bo nie było też w "fsmar.cpp", gdzie to
   * miało chyba jakieś znaczenie. Do wyjaśnienia? */
  if(dodatkowy_eof)
  {
   pracuj = 0; /* Będziemy kończyć działanie */
   /* To tylko zmienia stan aktualnego ogniwa */
   ZmienStan(STATE_EOF, KIER_PROSTO);
   status = NextProcess->Work(SAND_EOF, NULL, KIER_PROSTO);
  }
  if(pobieraj_dane)
  {
   cykl_pobierania = 1;
   while(cykl_pobierania)
   {
    assert(BufSize - BufDanych > 0);
    r1 = PrevProcess->Work(SAND_DR, BufPtr + BufDanych, BufSize - BufDanych);
    if(r1 != RESULT_OFF)
    {
     if(r1 != RESULT_EOF)
     {
      if(r1 != RESULT_AGAIN)
      {
       /* Tutaj "r1" to liczba pobranych znaków */
       BufDanych += r1;
       if(BufDanych == BufSize)
       {
        /* Wychodzimy z pętli pobierania danych, aby wysłać dane dotychczas
         * zgromadzone */
        cykl_pobierania = 0;
        wyslij_buf = 1;
       }
      }
      else
      {
       /* Chwilowo opuszczamy pętlę pobierania danych oraz główną pętlę,
        * wrócimy tu następnym razem */
       status = RESULT_AGAIN;
       cykl_pobierania = 0;
       pracuj = 0;
      }
     }
     else
     {
      /* Koniec cyklu pobierania, ale jeszcze nie kończymy pracy w głównej
       * pętli, ponieważ trzeba wysłać ewentualne dane oraz dać znać, że nowych
       * danych już nie będzie */
      cykl_pobierania = 0;
      if(BufDanych > 0)
      {
       wyslij_buf = 1; /* Wyślij jeszcze zgromadzone dane */
      }
      dodatkowy_eof = 1; /* Daj znać o końcu strumienia danych */
     }
    }
    else
    {
     SygError("Zawiódł wcześniejszy proces.");
     ZmienStan(STATE_OFF, KIER_PROSTO);
     cykl_pobierania = 0; /* Koniec cyklu pobierania */
     pracuj = 0;
    }
   }
  }
  /* Poniższy fragment możemy chcieć wykonywać wielokrotnie */
  if(wyslij_buf)
  {
   wyslij_buf = 0; /* Skasuj flagę, która nas tutaj wpuściła */
   suma = 0; /* Suma wysłanych bajtów */
   status = RESULT_AGAIN; /* Jeśli pojawi się błąd, to status się zmieni */
   while(suma < BufDanych)
   {
    r2 = NextProcess->Work(SAND_SR, BufPtr + suma, BufDanych - suma);
    if(r2 != RESULT_OFF)
    {
     assert(r2 >= 0);
     assert(r2 > 0); /* Podwójna asercja, aby wykryć zero */
     suma += r2;
     assert(suma <= BufDanych);
    }
    else
    {
     SygError("Drugi proces zawiódł");
     ZmienStan(STATE_OFF, KIER_WSTECZ);
     status = RESULT_OFF;
     pracuj = 0;
     break; /* Wyjście z pętli "while ()" */
    }
   }
   if(suma == BufDanych)
   {
    BufDanych = 0; /* Wysłaliśmy wszystkie dane */
   }
   else
   {
    SygErrorParm("Kopier nie wysłał wszystkich bajtów z bufora: %d", suma);
    /* Chcemy ubić następne moduły */
    ZmienStan(STATE_OFF, KIER_PROSTO);
    status = RESULT_OFF;
    pracuj = 0;
   }
   if(BufDelayD != 0)
   {
#ifdef PLATFORM_WIN
    Sleep(BufDelayD);
#endif
#ifdef PLATFORM_LINUX
    /* "usleep()" wymaga podania parametru w mikrosekundach */
    usleep(1000 * BufDelayD);
#endif
   }
  }
 }
 return status;
}

int kopier_Class::Obsluga_SAND_OFF(int kmn, Byte * poczatek, int ile)
{
 assert(ile == KIER_PROSTO);
 ZmienStan(STATE_OFF, ile);
 return RESULT_OFF;
}
