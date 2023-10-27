#ifndef _FGI_H
#define _FGI_H

#include "typy.h"



#include "procx.h"

class gi_Class : public ProcessClass
{
    #ifdef PLATFORM_LINUX
    long long
    #endif
    #ifdef PLATFORM_WIN
    __int64
    #endif
    Pobrane; /* Ile danych wysłaliśmy */

    #ifdef PLATFORM_LINUX
    long long
    #endif
    #ifdef PLATFORM_WIN
    __int64
    #endif
    Suma; /* Ile mamy wszystkich danych */

    /*
    Mogą być dwa tryby generowania danych:
    1. śmieciami (liczbami generowanymi tak, aby nie dało się ich
    skompresować - to pozwoli na ominięcie kłopotów z kompresującymi systemami
    plików (Win2000), co fałszowało wyniki
    2. stałą wartością bajtu podanego z linii komend - Win2000 reagowało błędnie na
    dane binarne, zrywając transmisję. Za pomocą tej opcji można wymusić np.
    spację lub literę "A" jako wzorzec generowanych danych i wtedy Win2000
    powinien poprawnie transmitować dane.
    */

    int Losowe; /* Czy dane mają być losowe (1) lub o stałej wartości (0) */
    /* Tym bajtem będzie wypełniany cały obszar danych, o ile Losowe=0 */
    int Wzorzec;

    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    gi_Class(void);
};

#endif
