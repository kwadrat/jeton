#include "fgi.h"


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/

gi_Class::gi_Class(void)
{
 RodzajWej = '-';
 RodzajWyj = 'B';
 Losowe = 1; /* Zakładamy, że dane będą generowane losowo */
}

/******************************************************************************/

void gi_Class::LocalDestructor(void)
{
 if(NextProcess != NULL)
 {
  NextProcess->LocalDestructor();
  delete NextProcess;
  NextProcess = NULL;
 }
}

/******************************************************************************/

int gi_Class::Init(int argc, char * argv[])
{
 int status;
 int nr; /* Numer argumentu aktualnie przetwarzanego */
 int poprawnie; /* Flaga informująca, że na razie jest wszystko poprawnie */

 status = RESULT_OFF;
 nr = 1; /* Aktualnie analizowana opcja */
 poprawnie = 1; /* Na razie nie wykryliśmy żadnych problemów */
 if(argc >= 2)
 {
  if( ! strcmp(argv[nr], "-b"))
  {
   nr ++; /* Przesuwamy się na wartość bajtu do wypełniania bufora */
   /* Wymagamy co najmniej 4 parametrów: "gi", "-b", "N", "rozmiar" */
   if(argc >= 4)
   {
    if(sscanf(argv[nr], "%d", &Wzorzec) == 1)
    {
     Losowe = 0; /* Będziemy generować stały wzorzec zamiast losowych liczb */
     nr ++; /* Omijamy jeden parametr */
    }
    else
    {
     SygErrorParm("Błąd w czasie próby dekodowania liczby z napisu \"%s\"",
                  argv[nr]);
     poprawnie = 0;
    }
   }
   else
   {
    SygError("Brak argumentów, \"gi\" potrzebuje \"-b\", \"N\", \"rozmiar\"");
    poprawnie = 0;
   }
  }
  if(poprawnie)
  {
#ifdef PLATFORM_LINUX
   sscanf(argv[nr], "%lld", & Suma);
#endif

#ifdef PLATFORM_WIN
   sscanf(argv[nr], "%I64d", & Suma);
#endif
   if(Suma > 0)
   {
    Pobrane = 0;
    status = nr + 1;
   }
   else
   {
    SygErrorParm("Nie udało się uzyskać dodatniej liczby z napisu \"%s\"",
                 argv[nr]);
   }
  }
 }
 else
 {
  SygError("Brak argumentu - liczby bajtów do wygenerowania");
 }
 return status;
}

/******************************************************************************/

int gi_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int status;

#ifdef PLATFORM_LINUX
 long long
#endif
#ifdef PLATFORM_WIN
 __int64
#endif
 kes;

 int r1;
 static int cnt2 = 1234; /* Ta wartość jest początkowa, i się później zmienia */
 int cnt1, cnt3;
 status = RESULT_EOF;
 switch(kmn)
 {
  case SAND_GENERAL:
  {
   if(NextProcess != NULL)
   {
    do
    {
     r1 = NextProcess->Work(SAND_GENERAL, NULL, KIER_PROSTO);
     assert(r1 < 0);
    }
    while(r1 == RESULT_AGAIN);
   }
   else
   {
    SygError("Brak następnego procesu");
   }
   break;
  }
  case SAND_DR:
  {
   if(State == STATE_ON)
   {
    if(ile > 0)
    {
     if(Pobrane > Suma)
     {

#ifdef PLATFORM_LINUX
      SygErrorParm("Pobrane zostało %lld bajtów (za dużo!)", Pobrane);
#endif

#ifdef PLATFORM_WIN
      SygErrorParm("Pobrane zostało %I64d bajtów (za dużo!)", Pobrane);
#endif

      kes =
#ifdef PLATFORM_LINUX
      (long long)
#endif
#ifdef PLATFORM_WIN
      (__int64)
#endif
      RESULT_OFF;

     }
     else
     {
      if(Pobrane == Suma)
      {
       kes =
#ifdef PLATFORM_LINUX
       (long long)
#endif
#ifdef PLATFORM_WIN
       (__int64)
#endif
       RESULT_EOF;
      }
      else
      {
       kes = Suma - Pobrane;
       if
       (
        (
#ifdef PLATFORM_LINUX
         (long long)
#endif
#ifdef PLATFORM_WIN
         (__int64)
#endif
         ile
        )
       < kes
       )
       {
        kes =
#ifdef PLATFORM_LINUX
         (long long)
#endif
#ifdef PLATFORM_WIN
         (__int64)
#endif
        ile;
       }
       Pobrane += kes;
       if(Losowe)
       {
        /* Dane generowane pseudolosowo */
        cnt1 = (((int) kes) / sizeof(int)) - 1;
        cnt3 = 0; /* Przyrost */
        for(; cnt1 >= 0; cnt1 --)
        {
         ((int *) poczatek )[cnt1] = (cnt2 += (cnt3++));
        }
       }
       else
       {
        memset(poczatek, Wzorzec, (int) kes);
       }
      }
     }
     status = (int) kes;
    }
    else
    {
     SygErrorParm("Mieliśmy otrzymać bufor o dodatniej wielkości: %d", ile);
     State = STATE_EOF;
    }
   }
   else
   {
    status = RESULT_EOF;
   }
   break;
  }
  default:
  {
   SygErrorParm("Nieznany typ komunikatu %d\n", kmn);
   ZmienStan(STATE_OFF, KIER_INNY);
  }
 }
 return status;
}
