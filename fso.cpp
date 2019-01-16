#include "fso.h"
#include "typy.h"
#include <stdio.h>

/******************************************************************************/

so_Class::so_Class(void)
{
 RodzajWej = 'B';
 RodzajWyj = '-';
}

/******************************************************************************/

void so_Class::LocalDestructor(void)
{
}

/******************************************************************************/

int so_Class::Init(int, char *[])
{
 return 1;
}

/******************************************************************************/

int so_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int kes;
 int suma;
 int status;

 status = RESULT_OFF;
 switch(kmn)
 {
  case SAND_SR:
  {
   suma = 0;
   while(suma < ile)
   {
    kes = fwrite(poczatek + suma, sizeof(Byte), ile - suma, stdout);
    if(kes > 0)
    {
     suma += kes;
    }
    else
    {
     SygError("Błąd zapisu do standardowego wyjścia");
     return RESULT_OFF;
    }
   }
   status = suma;
   break;
  }
  case SAND_EOF:
  {
   status = RESULT_EOF;
   break;
  }
  case SAND_OFF:
  {
   ZmienStan(STATE_OFF, KIER_PROSTO);
   status = RESULT_OFF;
   break;
  }
  default:
  {
   SygErrorParm("Nieznany komunikat: %d", kmn);
   ZmienStan(STATE_OFF, KIER_PROSTO);
  }
 }
 return status;
}

/******************************************************************************/
