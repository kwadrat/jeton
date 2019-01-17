#include "fy.h"
#include "typy.h"
#include <assert.h>

/******************************************************************************/

y_Class::y_Class(void)
{
 RodzajWej = 'B';
 RodzajWyj = 'M';
 OtherProcess = NULL;
}

/******************************************************************************/

void y_Class::LocalDestructor(void)
{
 if(NextProcess != NULL)
 {
  NextProcess->LocalDestructor();
  delete NextProcess;
  NextProcess = NULL;
 }
 if(OtherProcess != NULL)
 {
  OtherProcess->LocalDestructor();
  delete OtherProcess;
  OtherProcess = NULL;
 }
}

/******************************************************************************/

int y_Class::Init(int, char *[])
{
 /* Zużywamy tylko jeden argument, a nie potrzebujemy się specjalnie
  * inicjalizować */
 return 1;
}

/******************************************************************************/

int y_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int status;

 status = RESULT_EOF;
 switch(kmn)
 {
  case SAND_OFF:
  {
   status = Obsluga_SAND_OFF(kmn, poczatek, ile);
   break;
  }
  case SAND_EOF:
  {
   status = Obsluga_SAND_EOF(kmn, poczatek, ile);
   break;
  }
  case SAND_SR:
  {
   status = Obsluga_SAND_SR(kmn, poczatek, ile);
   break;
  }
  default:
  {
   SygErrorParm("Nieznany typ komunikatu: %d", kmn);
   ZmienStan(STATE_OFF, KIER_INNY);
   status = RESULT_OFF;
   break;
  }
 }
 return status;
}

/******************************************************************************/

ProcessClass * y_Class::FindUnusedPointer(void)
{
 ProcessClass * tmp;

 tmp = NULL;
 if(NextProcess != NULL)
 {
  tmp = NextProcess->FindUnusedPointer();
  if(tmp == NULL)
  {
   if(OtherProcess != NULL)
   {
    tmp = OtherProcess->FindUnusedPointer();
   }
   else
   {
    if(RodzajWyj == 'M' || RodzajWyj == 'B')
    {
     tmp = this;
    }
   }
  }
 }
 else
 {
  if(RodzajWyj == 'M' || RodzajWyj == 'B')
  {
   tmp = this;
  }
 }
 return tmp;
}

/******************************************************************************/

/* Podłącza w wolne miejsce wskaźnik następnego procesu. Wartość zwracana:
1 - OK, 0 - błąd */
int y_Class::ConnectPointer(ProcessClass * proc)
{
 int status;
 status = 0;
 if(NextProcess == NULL)
 {
  NextProcess = proc;
  proc->PrevProcess = this;
  status = 1;
 }
 else
 {
  if(OtherProcess == NULL)
  {
   OtherProcess = proc;
   proc->PrevProcess = this;
   status = 1;
  }
  else
  {
   SygError("Proces twierdził, że ma miejsce na podłączenie następnego?");
  }
 }
 return status;
}

/******************************************************************************/

/* Zmienia stan obiektu na podany. Jeśli to jest zmiana na STATE_OFF,
to od razu powiadamia sąsiadów z obu stron. */
void y_Class::ZmienStan(int nowy, int kier)
{
 assert(nowy == STATE_OFF || nowy == STATE_EOF);
 assert(kier == KIER_PROSTO || kier == KIER_WSTECZ || kier == KIER_INNY);
 if(nowy == STATE_OFF)
 {
  if(State != nowy)
  {
   State = nowy;
   if(kier != KIER_WSTECZ)
   {
    if(NextProcess != NULL)
    {
     NextProcess->Work(SAND_OFF, NULL, KIER_PROSTO);
    }
    if(OtherProcess != NULL)
    {
     OtherProcess->Work(SAND_OFF, NULL, KIER_PROSTO);
    }
   }
  }
 }
 else
 {
  State = nowy;
 }
}

/******************************************************************************/

int y_Class::Obsluga_SAND_OFF(int kmn, Byte * poczatek, int ile)
{
 ZmienStan(STATE_OFF, ile);
 return RESULT_OFF;
}

/******************************************************************************/

int y_Class::Obsluga_SAND_EOF(int kmn, Byte * poczatek, int ile)
{
 ZmienStan(STATE_EOF, KIER_PROSTO);
 assert(poczatek == NULL);
 assert(ile == KIER_PROSTO);
 NextProcess->Work(kmn, poczatek, ile);
 OtherProcess->Work(kmn, poczatek, ile);
 return RESULT_EOF; /* To chyba nie jest istotne, bo na to nikt nie czeka */
}

/******************************************************************************/

int y_Class::Obsluga_SAND_SR(int kmn, Byte * poczatek, int ile)
{
 int status;
 int r1, r2, suma;

 assert(poczatek != NULL);
 assert(ile > 0);
 status = RESULT_OFF; /* Jeśli coś nie wyjdzie, to od razu zrywamy łańcuch */
 if(State != STATE_OFF)
 {
  r1 = NextProcess->Work(kmn, poczatek, ile);
  if(r1 != RESULT_OFF)
  {
   assert(r1 >= 0);
   if(r1 != 0)
   {
    suma = 0;
    status = r1; /* Wstępnie zwracamy liczbę przetworzonych bajtów */
    do
    {
     r2 = OtherProcess->Work(kmn, poczatek, r1 - suma);
     if(r2 != RESULT_OFF)
     {
      assert(r2 >= 0);
      assert(r2 > 0); /* Dwie asercje, żeby wykryć przypadek zera */
      suma += r2;
      assert(suma <= r1);
     }
     else
     {
      SygError("Drugi proces zawiódł");
      ZmienStan(STATE_OFF, KIER_INNY);
      status = RESULT_OFF;
      break; /* Wyjście z pętli do {} while (); */
     }
    }
    while(suma < r1);
   }
   else
   {
    /* Nie zjadł ani bajta - dlaczego? */
    SygError("Nie zostały pobrane żadne dane");
    ZmienStan(STATE_OFF, KIER_INNY);
   }
  }
  else
  {
   ZmienStan(STATE_OFF, KIER_INNY);
  }
 }
 return status;
}

/******************************************************************************/

/* Tworzenie nowych wątków dla obsługi systemu i budzenie ich.
Wartość zwrotna: 1 - OK (udało się pomyślnie utworzyć nowe wątki),
0 - błąd (możemy kończyć pracę systemu, bo się nie udał przydział wątków */
int y_Class::Rozszczepianie(void)
{
 int status;
 status = 1; /* Tak ogólnie niczego nie trzeba rozszczepiać */
 if(NextProcess != NULL)
 {
  status = NextProcess->Rozszczepianie();
  if(status == 1)
  {
   if(OtherProcess != NULL)
   {
    status = OtherProcess->Rozszczepianie();
   }
  }
 }
 return status;
}

/******************************************************************************/
