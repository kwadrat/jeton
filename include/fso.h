#ifndef _FSO_H
#define _FSO_H

#include "procx.h"

class so_Class : public ProcessClass
{
    public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    so_Class(void);
};

#endif
