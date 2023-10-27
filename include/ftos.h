#ifndef _FTOS_H
#define _FTOS_H

#include "typy.h"

#if AKTYWNY_RIVER

#include "rvrx.h"
#include "procx.h"

class tos_Class : public ProcessClass
{
    int plik; /* Identyfikator połączenia TCP */

    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    tos_Class(void);
};

#endif
#endif
