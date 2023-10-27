#ifndef _FTIC_H
#define _FTIC_H

#include "typy.h"

#if AKTYWNY_RIVER

#include "rvrx.h"
#include "procx.h"

class tic_Class : public ProcessClass
{
    int plik; /* Identyfikator połączenia TCP */

    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    tic_Class(void);
};

#endif
#endif
