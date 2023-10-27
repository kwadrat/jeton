#include "fsi.h"
#include "typy.h"
#include <stdio.h>
#include <assert.h>

si_Class::si_Class(void)
{
    RodzajWej = '-';
    RodzajWyj = 'B';
}

void si_Class::LocalDestructor(void)
{
    if(NextProcess != NULL)
    {
        NextProcess->LocalDestructor();
        delete NextProcess;
        NextProcess = NULL;
    }
}

int si_Class::Init(int, char *[])
{
    return 1;
}

int si_Class::Work(int kmn, Byte * poczatek, int ile)
{
    int status;
    int kes;
    int r1;
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
                    kes = fread(poczatek, sizeof(Byte), ile, stdin);
                    if(kes > 0)
                    {
                        status = kes;
                    }
                    else
                    {
                        status = RESULT_EOF;
                        State = STATE_EOF;
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
