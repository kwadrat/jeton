#include "ftos.h"

#if AKTYWNY_RIVER
#include <stdio.h>
#include <errno.h>

tos_Class::tos_Class(void)
{
    RodzajWej = 'B';
    RodzajWyj = '-';
    plik = -1;
}

void tos_Class::LocalDestructor(void)
{
    if(plik != -1)
    {
        TopRiver.CloseSocketTCP(plik);
        plik = -1;
    }
}

int tos_Class::Init(int argc, char * argv[])
{
    int status;
    status = RESULT_OFF;
    if(argc >= 2)
    {
        plik = TopRiver.OpenDaemonTCP(argv[1]);
        if(plik >= 0)
        {
            status = 2;
        }
        else
        {
            SygError("Zawiodła funkcja OpenDaemonTCP");
        }
    }
    else
    {
        SygError("Brakuje parametru - portu TCP, na którym mamy nasłuchiwać");
    }
    return status;
}

int tos_Class::Work(int kmn, Byte * poczatek, int ile)
{
    int suma;
    int status;

    status = RESULT_OFF;
    switch(kmn)
    {
        case SAND_SR:
        {
            suma = TopRiver.SendMultiTCP(plik, poczatek, ile);
            if(suma > 0)
            {
                status = suma;
            }
            else
            {
                SygError("Błąd zapisu do pliku wyjściowego ");
                ZmienStan(STATE_OFF, KIER_PROSTO);
                return RESULT_OFF;
            }
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

#endif
