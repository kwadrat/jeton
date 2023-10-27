#ifndef _MASTERX_H
#define _MASTERX_H

#include "typy.h"
#include "procx.h"

class MasterClass
{
    ProcessClass * HeadPtr;

    public:
    MasterClass(void);
    ~MasterClass(void);
    int AddProcess(ProcessClass *);
    int CheckComplete(void);
    void RunProcesses(void);
    virtual void LocalDestructor(void);
};

#endif
