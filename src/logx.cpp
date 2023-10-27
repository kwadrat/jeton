#include "typy.h"

#include <stdio.h>
#include <string.h>

#include "logx.h"
#include "masterx.h"
#include "procx.h"
#include "fsi.h"
#include "fso.h"
#include "fkopier.h"
#include "fy.h"
#include "fsmar.h"
#include "fcat.h"
#include "ffi.h"
#include "ffo.h"
#include "ftoc.h"
#include "ftis.h"
#include "ftos.h"
#include "ftic.h"
#include "fgi.h"
#include "facptr.h"
#include "fmd5.h"


extern MasterClass TopMember;

#define IleArgumentow (n - index) /* Liczba pozostałych argumentów */

/* Funkcja do przetwarzania wprowadzonych z klawiatury i powtarzających się
* sekwencji analizy napisu wejściowego. Wartość zwracana: 1 - OK, 0 - wystąpił
* błąd, który nie pozwala na kontynuację działania. */
int CreateChainLink(void * ogniwo, const char * NazwaTypu, int * index, int n,
char * s[])
{
    int status;
    ProcessClass * tmp; /* Tu wstawiamy kolejno utworzone ogniwa */
    int IleArg; /* Liczba argumentów zużywanych przez ogniwo */
    static char Komunikat[80];

    status = 1;
    tmp = (ProcessClass *) ogniwo;
    if(tmp != NULL)
    {
        IleArg = tmp->Init((n - (*index)), s + (*index));
        if(IleArg == RESULT_OFF)
        {
            SygErrorParm("Błąd podczas inicjalizacji obiektu %s", NazwaTypu);
            status = 0;
        }
        else
        {
            if( ! TopMember.AddProcess(tmp))
            {
                SygErrorParm("Błąd przy próbie dołączenia obiektu %s.", NazwaTypu);
                status = 0;
            }
            (*index) += IleArg;
        }
    }
    else
    {
        sprintf(Komunikat, "Błąd tworzenia obiektu %s.", NazwaTypu);
        SygError(Komunikat);
        status = 0; /* Błąd przetwarzania */
    }
    return status;
}

/* Funkcja sprawdza, czy łańcuch jest kompletny. Łańcuch jest taki, jeśli nie
* można już dodać żadnego ogniwa. Wartość zwracana: 1 - łańcuch jest
* kompletny, 0 - do łańcucha da się jeszcze coś dodać, co oznacza że
* użytkownik nie zakończył odpowiednio tego łańcucha i program nie może dalej
* pracować. */
int CheckChainComplete(void)
{
    return TopMember.CheckComplete();
}

/* Funkcja tworzy łańcuch (a właściwie drzewo, bo mogą w nim występować
rozgałęzienia) struktur (obiektów pochodnych klasy ProcessClass). Jeśli
w czasie tworzenia łańcucha wystąpi błąd, to zwracana jest wartość 0,
w przeciwnym razie wartość 1. */
int CreateChainLogic(int n, char * s[])
{
    int status;
    int index; /* Numer aktualnie przetwarzanego argumentu */

    status = 1; /* Na razie nie było żadnego błędu */
    index = 0; /* Początkowy indeks elementu */
    while(index < n && status)
    {
        if( ! strcmp(s[index], "si"))
        {
            status = CreateChainLink(new si_Class, "si_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "so"))
        {
            status = CreateChainLink(new so_Class, "so_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "kopier"))
        {
            status = CreateChainLink(new kopier_Class, "kopier_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "y"))
        {
            status = CreateChainLink(new y_Class, "y_Class", & index, n, s);
        }
        #if AKTYWNY_FSMAR
        else if( ! strcmp(s[index], "smar"))
        {
            status = CreateChainLink(new smar_Class, "smar_Class", & index, n, s);
        }
        #endif
        #if AKTYWNY_FCAT
        else if( ! strcmp(s[index], "cat"))
        {
            status = CreateChainLink(new cat_Class, "cat_Class", & index, n, s);
        }
        #endif
        else if( ! strcmp(s[index], "fi"))
        {
            status = CreateChainLink(new fi_Class, "fi_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "fo"))
        {
            status = CreateChainLink(new fo_Class, "fo_Class", & index, n, s);
        }
        #if AKTYWNY_RIVER
        else if( ! strcmp(s[index], "toc"))
        {
            status = CreateChainLink(new toc_Class, "toc_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "tis"))
        {
            status = CreateChainLink(new tis_Class, "tis_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "tos"))
        {
            status = CreateChainLink(new tos_Class, "tos_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "tic"))
        {
            status = CreateChainLink(new tic_Class, "tic_Class", & index, n, s);
        }
        #endif
        else if( ! strcmp(s[index], "gi"))
        {
            status = CreateChainLink(new gi_Class, "gi_Class", & index, n, s);
        }
        else if( ! strcmp(s[index], "acptr"))
        {
            status = CreateChainLink(new acptr_Class, "acptr_Class", & index, n, s);
        }
        #if AKTYWNY_MD5
        else if( ! strcmp(s[index], "md5"))
        {
            status = CreateChainLink(new md5_Class, "md5_Class", & index, n, s);
        }
        #endif
        else
        {
            SygErrorParm("Nierozpoznana opcja \"%s\".", s[index]);
            status = 0;
        }
    }
    if(status)
    {
        status = CheckChainComplete();
        if( ! status)
        {
            SygError("Łańcuch nie jest kompletny - brak zakończenia.");
        }
    }
    return status;
}
