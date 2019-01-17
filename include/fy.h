#ifndef _FY_H
#define _FY_H

#include "procx.h"

class y_Class : public ProcessClass
{
public:
 y_Class(void);
 ProcessClass * OtherProcess;
 virtual int Init(int, char *[]);
 virtual int Work(int, Byte *, int);
 virtual void LocalDestructor(void);
 virtual ProcessClass * FindUnusedPointer(void);
 virtual int ConnectPointer(ProcessClass *);
 virtual void ZmienStan(int, int);
 virtual int Rozszczepianie(void);
 int Obsluga_SAND_OFF(int, Byte *, int);
 int Obsluga_SAND_EOF(int, Byte *, int);
 int Obsluga_SAND_SR(int, Byte *, int);
};
#endif

