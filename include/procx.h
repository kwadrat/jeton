#ifndef _PROCX_H
#define _PROCX_H

#include "typy.h"

class ProcessClass
{
    protected:
    int State;
    int RodzajWej;
    int RodzajWyj;

    public:
    ProcessClass(void);
    virtual ~ProcessClass(void);
    ProcessClass * PrevProcess;
    ProcessClass * NextProcess;
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    virtual ProcessClass * FindUnusedPointer(void);
    virtual int ConnectPointer(ProcessClass *);
    virtual void ZmienStan(int, int);
    virtual int Rozszczepianie(void);
    int ZgodnyTyp(ProcessClass *);
    int ProcesWejsciowy(void);
    int DodajWatek(void);
    int FunkcjaWatka(void);
};

#endif
