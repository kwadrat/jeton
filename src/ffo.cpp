#include "ffo.h"
#include "typy.h"
#include "rvrx.h"
#include <stdio.h>
#include <errno.h>

fo_Class::fo_Class(void)
{
    RodzajWej = 'B';
    RodzajWyj = '-';
    plik = -1;
}

void fo_Class::LocalDestructor(void)
{
    if(plik != -1)
    {
        close(plik);
        plik = -1;
    }
}

int fo_Class::Init(int argc, char * argv[])
{
    int status;
    status = RESULT_OFF;
    if(argc >= 2)
    {
        plik = open(argv[1], ZAPIS_BINARNY, PRAWA_ZAPISU);
        if(plik != -1)
        {
            status = 2;
        }
        else
        {
            SygErrorParm("Funkcja open() zwróciła %d\n", errno);
        }
    }
    else
    {
        SygError("Brakuje parametru, nazwy pliku do otwarcia");
    }
    return status;
}

int fo_Class::Work(int kmn, Byte * poczatek, int ile)
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
                kes = write(plik, poczatek + suma, ile - suma);
                if(kes > 0)
                {
                    suma += kes;
                }
                else
                {
                    SygError("Błąd zapisu do pliku wyjściowego ");
                    ZmienStan(STATE_OFF, KIER_PROSTO);
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
