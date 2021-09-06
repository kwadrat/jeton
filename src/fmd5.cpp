#include "fmd5.h"

#if AKTYWNY_MD5


#include <stdlib.h>
#include <assert.h>

/******************************************************************************/

md5_Class::md5_Class(void)
{
    RodzajWej = 'M';
    RodzajWyj = 'M';
    BufSize = 0;
    BufPtr = NULL;
}

/******************************************************************************/

void md5_Class::LocalDestructor(void)
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
    }
}

/******************************************************************************/

/* Jedynym parametrem jest liczba bajtów w buforze */
int md5_Class::Init(int argc, char *argv[])
{
    int ile;
    int status;

    status = RESULT_OFF;
    return status;
}

/******************************************************************************/

int md5_Class::Work(int kmn, Byte * poczatek, int ile)
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

/******************************************************************************/

int md5_Class::Obsluga_SAND_GENERAL(int kmn, Byte * poczatek, int ile)
{
    int status;

    status = RESULT_OFF;
    return status;
}

/******************************************************************************/

int md5_Class::Obsluga_SAND_OFF(int kmn, Byte * poczatek, int ile)
{
    assert(ile == KIER_PROSTO);
    ZmienStan(STATE_OFF, ile);
    return RESULT_OFF;
}

/******************************************************************************/

#endif
