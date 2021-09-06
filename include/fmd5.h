#ifndef _FMD5_H
#define _FMD5_H

#include "typy.h"

#if AKTYWNY_MD5

#include "procx.h"

#include <string.h>

class md5_Class : public ProcessClass
{
    int BufSize;
    Byte * BufPtr;
    int BufIle; /* Liczba danych w buforze */
    void StartingValues(void);
    void ProcessFullBlock(int bt_count);
    void ProcessPartialBlock(int bt_count);
    unsigned int AddBuff;
    unsigned int XorBuff;
    unsigned int SubBuff;

public:
    virtual int Init(int, char *[]);
    virtual int Work(int, Byte *, int);
    virtual void LocalDestructor(void);
    md5_Class(void);
    int Obsluga_SAND_GENERAL(int, Byte *, int);
    int Obsluga_SAND_OFF(int, Byte *, int);
};

#endif
#endif
