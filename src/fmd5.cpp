#include "fmd5.h"

#if AKTYWNY_MD5

#include <stdlib.h>
#include <assert.h>

md5_Class::md5_Class(void)
{
    RodzajWej = 'M';
    RodzajWyj = 'M';
    BufSize = 0;
    BufPtr = NULL;
}

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

/* Jedynym parametrem jest liczba bajtów w buforze */
int md5_Class::Init(int argc, char *argv[])
{
    int ile;
    int status;

    status = RESULT_OFF;
    if(argc >= 2)
    {
        ile = atoi(argv[1]);
        if(ile > 0)
        {
            BufSize = ile;
            if((BufSize & 3) == 0)
            {
                BufPtr = (Byte *) malloc((BufSize) * sizeof(Byte));
                if(BufPtr != NULL)
                {
                    StartingValues();
                    BufIle = 0;
                    status = 2;
                }
                else
                {
                    SygErrorParm("Brakło %zu bajtów pamięci", (BufSize + 72) * sizeof(Byte));
                }
            }
            else
            {
                SygErrorParm("Rozmiar bufora %d nie jest podzielny przez 4", BufSize);
            }
        }
        else
        {
            SygErrorParm("Nie udało się wczytać wielkości bufora z napisu %s", argv[1]);
        }
    }
    else
    {
        SygError("Brakuje parametru - rozmiaru bufora");
    }
    return status;
}

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

int md5_Class::DisplayState(void)
{
    int limit; /* Tyle znaków trzeba wysyłać do następnego procesu */
    int suma; /* Liczba znaków dotychczas wysłanych */
    char FnlText[32]; /* Suma kontrolna - powinno wystarczyć na 3x8, spacje, CR/LF, końcowe zero */
    int status = RESULT_ALL_SENT;
    int r2;
    sprintf(FnlText, "%08x %08x %08x\n", AddBuff, XorBuff, SubBuff);
    limit = strlen(FnlText);
    suma = 0;
    do
    {
        r2 = NextProcess->Work(SAND_SR, (Byte *) FnlText + suma, limit - suma);
        if(r2 != RESULT_OFF)
        {
            assert(r2 >= 0);
            assert(r2 > 0); /* Podwójna asercja, aby wykryć zero */
            suma += r2;
            assert(suma <= limit);
        }
        else
        {
            SygError("Drugi proces zawiódł");
            ZmienStan(STATE_OFF, KIER_WSTECZ);
                status = RESULT_OFF;
                break; /* Wyjście z pętli do {} while (); */
        }
    }
    while(suma < limit);
    return status;
}

int md5_Class::Obsluga_SAND_GENERAL(int kmn, Byte * poczatek, int ile)
{
    int r1;
    int status;
    int i;
    int pracuj; /* Znacznik do pętli wypełniającej bufor danymi */

    status = RESULT_OFF;
    assert(PrevProcess != NULL);
    assert(NextProcess != NULL);
    pracuj = 1;
    while(pracuj)
    {
        r1 = PrevProcess->Work(SAND_DR, BufPtr + BufIle, BufSize - BufIle);
        if(r1 != RESULT_OFF)
        {
            if(r1 != RESULT_EOF)
            {
                if(r1 != RESULT_AGAIN)
                {
                    BufIle += r1;
                    if(BufIle == BufSize)
                    {
                        /* "BufIle" musi być podzielny przez 4 */
                        ProcessFullBlock(BufIle);
                        BufIle = 0;
                    }
                }
                else
                {
                    status = RESULT_AGAIN;
                    pracuj = 0;
                }
            }
            else
            {
                ZmienStan(STATE_EOF, KIER_PROSTO);
                if(BufIle > 0)
                {
                    ProcessPartialBlock(BufIle);
                }
                status = DisplayState();
                if(status == RESULT_ALL_SENT)
                {
                    status = NextProcess->Work(SAND_EOF, NULL, KIER_PROSTO);
                }
                pracuj = 0;
            }
        }
        else
        {
            SygError("Zawiódł wcześniejszy proces.");
            ZmienStan(STATE_OFF, KIER_PROSTO);
            exit(0); /* qaz - to chwilowo */
        }
    }
    return status;
}

int md5_Class::Obsluga_SAND_OFF(int kmn, Byte * poczatek, int ile)
{
    assert(ile == KIER_PROSTO);
    ZmienStan(STATE_OFF, ile);
    return RESULT_OFF;
}

void md5_Class::StartingValues(void)
{
    AddBuff = 0;
    XorBuff = 0;
    SubBuff = 0x9e3b0d12; /* Random number */
}

void md5_Class::ProcessFullBlock(int bt_count)
{
    int i;
    int word_cnt;
    int bit_is_set;
    unsigned int one_value;
    if((bt_count & 3) == 0)
    {
        word_cnt = bt_count >> 2;
        for(i = 0; i < word_cnt; i++)
        {
            one_value = ((unsigned int *)BufPtr)[i];
            AddBuff += one_value;
            XorBuff ^= one_value;
            SubBuff -= one_value;
            bit_is_set = SubBuff & (1 << 31);
            SubBuff <<= 1;
            if(bit_is_set)
            {
                SubBuff |= 1;
            }
        }
    }
    else
    {
        SygErrorParm("bt_count %d nie jest podzielny przez 4", bt_count);
    }
}

void md5_Class::ProcessPartialBlock(int bt_count)
{
    /* Wyzeruj bajty poza końcem bloku */
    while(bt_count & 3)
    {
        BufPtr[bt_count] = 0;
        bt_count++;
    }
    ProcessFullBlock(bt_count);
}

#endif
