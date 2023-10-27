#ifndef _FTOC_H
#define _FTOC_H

#include "typy.h"

#if AKTYWNY_RIVER

#include "rvrx.h"
#include "procx.h"

class toc_Class : public ProcessClass
{
    int plik; /* Identyfikator połączenia TCP */

    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    toc_Class(void);
};

#endif
#endif
