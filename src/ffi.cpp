
#include "ffi.h"
#include "rvrx.h"
#include <errno.h>
#include <string.h>


fi_Class::fi_Class(void)
{
    RodzajWej = '-';
    RodzajWyj = 'B';
    plik = -1;
    Suma = 0;
}

void fi_Class::LocalDestructor(void)
{
    if(plik != -1)
    {
        close(plik);
        plik = -1;
    }
    if(NextProcess != NULL)
    {
        NextProcess->LocalDestructor();
        delete NextProcess;
        NextProcess = NULL;
    }
}

int fi_Class::Init(int argc, char *argv[])
{
    int status;
    int nr; /* Numer argumentu aktualnie przetwarzanego */
    int poprawnie;
    status = RESULT_OFF;
    nr = 1;
    poprawnie = 1; /* Na razie wszystko działa poprawnie */
    if(argc >= 2)
    {
        if( ! strcmp(argv[nr], "-m"))
        {
            nr ++; /* Przesuwamy się na liczbę bajtów do odczytania */
            if(argc >= 4)
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
                    nr ++;
                }
                else
                {
                    SygErrorParm("Nie udało się uzyskać dodatniej liczby z napisu \"%s\"",
                    argv[nr]);
                    poprawnie = 0;
                }
            }
            else
            {
                SygError("Brak argumentów, \"fi\" potrzebuje \"-m\", \"N\", \"plik\"");
                poprawnie = 0;
            }
        }
        if(poprawnie)
        {
            //fprintf(stderr, "Próba otwarcia pliku \"%s\" z numeru %d\n", argv[nr], nr);
            plik = open(argv[nr], ODCZYT_BINARNY);
            if(plik != -1)
            {
                status = nr + 1; /* Zużywamy dwa lub cztery parametry */
            }
            else
            {
                SygErrorParm("Funkcja open() zwróciła %d", errno);
                SygErrorParm("przy próbie otwarcia pliku \"%s\"", argv[nr]);
            }
        }
    }
    else
    {
        SygError("Brakuje parametru, nazwy pliku do otwarcia");
    }
    return status;
}

int fi_Class::Work(int kmn, Byte * poczatek, int ile)
{
    int status;
    status = RESULT_EOF;
    if(State != STATE_OFF)
    {
        switch(kmn)
        {
            case SAND_GENERAL:
            {
                status = Obsluga_SAND_GENERAL(kmn, poczatek, ile);
                break;
            }
            case SAND_DR:
            {
                status = Obsluga_SAND_DR(kmn, poczatek, ile);
                break;
            }
            default:
            {
                SygErrorParm("Nieznany typ komunikatu: %d\n", kmn);
                break;
            }
        }
    }
    return status;
}

int fi_Class::Obsluga_SAND_GENERAL(int kmn, Byte * poczatek, int ile)
{
    int status;
    int pracuj;
    status = RESULT_OFF;
    pracuj = 1;
    while(pracuj)
    {
        status = NextProcess->Work(SAND_GENERAL, NULL, KIER_PROSTO);
        if(status != RESULT_AGAIN)
        {
            pracuj = 0;
        }
    }
    return status;
}

int fi_Class::Obsluga_SAND_DR(int kmn, Byte * poczatek, int ile)
{
    int status;
    int kes;

    kes = ile;
    if(Suma != 0)
    {
        if(Pobrane > Suma)
        {
            #ifdef PLATFORM_LINUX
            SygErrorParm("Pobrane zostało %lld bajtów (za dużo!)", Pobrane);
            #endif

            #ifdef PLATFORM_WIN
            SygErrorParm("Pobrane zostało %I64d bajtów (za dużo!)", Pobrane);
            #endif
            status = RESULT_OFF;
        }
        else
        {
            if
            (
            (
            #ifdef PLATFORM_LINUX
            (long long)
            #endif
            #ifdef PLATFORM_WIN
            (__int64)
            #endif
            kes
            )
            >= Suma - Pobrane
            )
            {
                kes = (int) (Suma - Pobrane);
            }
        }
    }
    if(kes > 0)
    {
        status = read(plik, poczatek, kes);
    }
    else
    {
        status = 0;
    }
    if(status == 0)
    {
        status = RESULT_EOF;
    }
    else
    {
        if(status < 0)
        {
            status = RESULT_OFF;
            SygErrorParm("Podczas odczytu z pliku wejściowego wystąpił błąd errno = %d\n",
            errno);
        }
        else
        {
            if(Suma != 0)
            {
                Pobrane +=
                #ifdef PLATFORM_LINUX
                (long long)
                #endif
                #ifdef PLATFORM_WIN
                (__int64)
                #endif
                status;
            }
        }
    }
    return status;
}
