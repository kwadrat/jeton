#ifndef _FKOPIER_H
#define _FKOPIER_H

#include "procx.h"

/* Dołączamy poniższy plik, żeby dla Windows dostać prototyp funkcji Sleep().
 * Przy bezpośrednim dołączeniu "windows.h" występowała kolizja w winsock.h
 * (wielokrotne definicje) */
#include "rvrx.h"

#ifdef PLATFORM_LINUX
#include <unistd.h>
#endif


class kopier_Class : public ProcessClass
{
 int BufSize; /* Rozmiar bufora */
 Byte * BufPtr; /* Początek bufora */
 int BufDanych; /* Liczba bajtów danych w buforze */
 int BufDelayD; /* Co tyle milisekund czekamy po każdej tranmisji */
public:
 virtual int Init(int, char *[]);
 virtual int Work(int, Byte *, int);
 virtual void LocalDestructor(void);
 kopier_Class(void);
 int Obsluga_SAND_GENERAL(int, Byte *, int);
 int Obsluga_SAND_OFF(int, Byte *, int);
};

#endif
