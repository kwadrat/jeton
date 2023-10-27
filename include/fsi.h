#ifndef _FSI_H
#define _FSI_H

#include "procx.h"

class si_Class : public ProcessClass
{
    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    si_Class(void);
};

#endif
