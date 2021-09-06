#include "fcat.h"

#if AKTYWNY_FCAT

#include "typy.h"

#include <string.h>

cat_Class::cat_Class(void)
{
 RodzajWej = 'B';
 RodzajWyj = 'B';
}

void cat_Class::LocalDestructor(void)
{
 if(NextProcess != NULL)
 {
  NextProcess->LocalDestructor();
  delete NextProcess;
  NextProcess = NULL;
 }
}

/* Nie potrzebujemy żadnych parametrów */
int cat_Class::Init(int argc, char *argv[])
{
 return 1;
}

int cat_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int status;

 status = RESULT_OFF;
 DIAGNA2("QAZ: cat_kmn = %d\n", kmn);
 if(State != STATE_OFF)
 {
  switch(kmn)
  {
   case SAND_DR:
   {
    status = Obsluga_SAND_DR(kmn, poczatek, ile);
    break;
   }
   case SAND_SR:
   {
    status = Obsluga_SAND_SR(kmn, poczatek, ile);
    break;
   }
   case SAND_EOF:
   {
    status = Obsluga_SAND_EOF(kmn, poczatek, ile);
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

int cat_Class::Obsluga_SAND_SR(int kmn, Byte * poczatek, int ile)
{
 int status;
 int s;

 assert(ile > 0);
 assert(poczatek != NULL);
 DIAGNA2("QAZ: cat_SAND_SR dostał %d\n", ile);
 status = RESULT_OFF;
 WskDanych = poczatek;
 SumaDanych = ile;
 Pobrano = 0;
 assert(NextProcess != NULL);
 s = NextProcess->Work(SAND_GENERAL, NULL, KIER_PROSTO);
 /* qaz - tutaj ignorujemy zmienną s. Dlaczego? */
 if(State != STATE_OFF)
 {
  status = Pobrano;
 }
 else
 {
  status = RESULT_OFF;
 }
 DIAGNA2("QAZ: cat_SAND_SR zwrócił %d\n", status);
 return status;
}

int cat_Class::Obsluga_SAND_DR(int kmn, Byte * poczatek, int ile)
{
 int status;
 int cnt; /* Liczba rzeczywiście zwróconych danych */

 assert(ile > 0);
 assert(poczatek != NULL);
 status = RESULT_OFF;
 DIAGNA3("QAZ: cat_SAND_DR zażądał %p %d\n", poczatek, ile);
 DIAGNA3("QAZ:1 cat_SAND_DR pobranych: %d wszystkich %d\n", Pobrano, SumaDanych);

 if(State != STATE_OFF)
 {
  if(State != STATE_EOF)
  {
   if(SumaDanych > Pobrano)
   {
    cnt = SumaDanych - Pobrano;
    if(cnt > ile)
    {
     cnt = ile;
    }
    memcpy(poczatek, WskDanych + Pobrano, cnt);
    Pobrano += cnt;
    status = cnt;
   }
   else
   {
    /* Skończyły się dane w buforze */
    assert(SumaDanych == Pobrano);
    SumaDanych = 0;
    WskDanych = NULL; /* Przestajemy udostępniać dane */
    status = RESULT_AGAIN;
   }
  }
  else
  {
   status = RESULT_EOF;
  }
 }
 else
 {
  ; /* Tu nic nie robimy - status już jest ustawiony odpowiednio */
 }
 DIAGNA3("QAZ:2 cat_SAND_DR pobranych: %d wszystkich %d\n", Pobrano, SumaDanych);
 DIAGNA2("QAZ: cat_SAND_DR zwraca %d\n", status);
 return status;
}

int cat_Class::Obsluga_SAND_EOF(int kmn, Byte * poczatek, int ile)
{
 int status;

 assert(ile == KIER_PROSTO);
 assert(NextProcess != NULL);
 status = RESULT_OFF;
 if(State != STATE_OFF)
 {
  status = RESULT_EOF;
  ZmienStan(STATE_EOF, KIER_PROSTO);
  /* Teraz damy szansę następnemu procesowi dowiedzieć się o końcu pliku */
  NextProcess->Work(SAND_GENERAL, NULL, KIER_PROSTO);
 }
 return status;
}

int cat_Class::Obsluga_SAND_OFF(int kmn, Byte * poczatek, int ile)
{
 assert(ile == KIER_PROSTO);
 ZmienStan(STATE_OFF, ile);
 return RESULT_OFF;
}

#endif
