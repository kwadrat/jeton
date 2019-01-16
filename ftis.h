#ifndef _FTIS_H
#define _FTIS_H

#include "typy.h"

#if AKTYWNY_RIVER

#include "rvrx.h"
#include "procx.h"

class tis_Class : public ProcessClass
{
 int plik; /* Identyfikator połączenia TCP */
public:
 virtual int Init(int, char *[]);
 virtual int Work(int, Byte *, int);
 virtual void LocalDestructor(void);
 tis_Class(void);
};

#endif
#endif
