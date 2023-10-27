#ifndef _FFO_H
#define _FFO_H

#include "procx.h"

class fo_Class : public ProcessClass
{
    int plik; /* Identyfikator pliku otwartego przez open() */

    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    fo_Class(void);
};

#endif
