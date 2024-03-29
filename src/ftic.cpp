#include "ftic.h"

#if AKTYWNY_RIVER
#include <stdio.h>
#include <assert.h>
#include <errno.h>

tic_Class::tic_Class(void)
{
    RodzajWej = '-';
    RodzajWyj = 'B';
}

void tic_Class::LocalDestructor(void)
{
    if(plik != -1)
    {
        TopRiver.CloseSocketTCP(plik);
        plik = -1;
    }
    if(NextProcess != NULL)
    {
        NextProcess->LocalDestructor();
        delete NextProcess;
        NextProcess = NULL;
    }
}

int tic_Class::Init(int argc, char * argv[])
{
    int status;
    status = RESULT_OFF;
    if(argc >= 3)
    {
        plik = TopRiver.OpenClientTCP(argv[1], argv[2]);
        if(plik >= 0)
        {
            status = 3;
        }
        else
        {
            SygError("Zawiodła funkcja OpenClientTCP");
        }
    }
    else
    {
        SygError("Brakuje parametru, nazwy hosta lub numeru portu TCP");
    }
    return status;
}

int tic_Class::Work(int kmn, Byte * poczatek, int ile)
{
    int status;
    int kes;
    int r1;
    int eof_flag;
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
                    eof_flag = 0;
                    kes = TopRiver.RecvMultiTCP(plik, poczatek, ile, & eof_flag);
                    if(eof_flag)
                    {
                        State = STATE_EOF; /* To były ostatnie dane z sieci */
                    }
                    if(kes > 0)
                    {
                        status = kes;
                    }
                    else
                    {
                        if(kes == 0)
                        {
                            status = RESULT_EOF;
                            State = STATE_EOF;
                        }
                        else
                        {
                            status = RESULT_OFF;
                            State = STATE_OFF;
                        }
                    }
                }
                else
                {
                    SygErrorParm("Mieliśmy otrzymać bufor o dodatniej wielkości: %d", ile);
                    State = STATE_EOF;
                }
            }
            else
            {
                if(State == STATE_EOF)
                {
                    status = RESULT_EOF;
                }
                else
                {
                    if(State == STATE_OFF)
                    {
                        status = RESULT_OFF;
                    }
                    else
                    {
                        SygErrorParm("Nieznany stan: %d", State);
                        State = STATE_OFF;
                    }
                }
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
#endif
