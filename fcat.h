#ifndef _FCAT_H
#define _FCAT_H

#include "typy.h"

#if AKTYWNY_FCAT

#include "procx.h"
#include <assert.h>

class cat_Class : public ProcessClass
{
 int Pobrano;
 int SumaDanych;
 Byte * WskDanych;
public:
 virtual int Init(int, char *[]);
 virtual int Work(int, Byte *, int);
 virtual void LocalDestructor(void);
 cat_Class(void);
 int Obsluga_SAND_DR(int, Byte *, int);
 int Obsluga_SAND_SR(int, Byte *, int);
 int Obsluga_SAND_EOF(int, Byte *, int);
 int Obsluga_SAND_OFF(int, Byte *, int);
};

#endif
#endif
