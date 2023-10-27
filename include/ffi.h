#ifndef _FFI_H
#define _FFI_H

#include "procx.h"

class fi_Class : public ProcessClass
{
    int plik; /* Identyfikator otwartego pliku za pomoca open() */

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

    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    int Obsluga_SAND_GENERAL(int, Byte *, int);
    int Obsluga_SAND_DR(int, Byte *, int);
    fi_Class(void);
};

#endif
