#ifndef _FACPTR_H
#define _FACPTR_H

#include "typy.h"


#include "rvrx.h"
#include "procx.h"
#ifdef PLATFORM_LINUX
#include <sys/time.h>
#include <unistd.h>
#endif

#ifdef PLATFORM_LINUX
typedef struct timeval TCZAS;
#endif
#ifdef PLATFORM_WIN
typedef DWORD TCZAS;
#endif

class acptr_Class : public ProcessClass
{
 /* Odstęp czasowy w sekundach (0 - nie robimy okresowych raportów */
 int t2;
 /* Suma bajtów w ogóle przesłanych */
#ifdef PLATFORM_LINUX
 long long
#endif
#ifdef PLATFORM_WIN
 __int64
#endif
 s0;
#ifdef PLATFORM_LINUX
 long long
#endif
#ifdef PLATFORM_WIN
 __int64
#endif
 s1, /* Liczba bajtów przesłana w ostatnim krótkim okresie */
 s_so_far, /* Sumaryczna liczba bajtów od początku transmisji */
 s3; /* Wielkość pobranej porcji danych */
 /* Różnica między początkiem i końcem okresu, zwracana przez funkcję
  * WyliczCzasDelta() */
 float tdiff;
 int RMax; /* Maksymalna liczba zarejestrowanych czasów */
 int RAkt; /* Aktualna liczba zarejestrowanych czasów */
 TCZAS t0; /* Moment rozpoczęcia całej transmisji */
 TCZAS t3; /* Początek krótkiego okresu */
 TCZAS t4; /* Prognozowany moment zakończenia krótkiego okresu */
 TCZAS t5; /* Tu umieszczamy właśnie odczytaną wartość zegara systemowego */
 TCZAS *RTab;
 /* Jeśli wartość jest niezerowa, to oznacza, co ile pobrań z poprzedzającego
  * magazynu jest wyświetlana statystyka */
 int SLmt;
 /* Dla niezerowej wartości "SLmt" tutaj jest wyznaczana aktualna liczba
  * odczytów z poprzedzającego magazynu */
 int SAkt;
public:
 virtual int Init(int, char *[]);
 virtual int Work(int, Byte *, int);
 virtual void LocalDestructor(void);
 acptr_Class(void);
 void AktualnyCzas(void);
 int Chronologicznie(void);
 void RaportOkresowy(void);
 void RaportKoncowy(void);
 void DumpRTable(void);
#ifdef PLATFORM_LINUX
 int WyliczCzasDelta(struct timeval *, struct timeval *);
#endif

#ifdef PLATFORM_WIN
 int WyliczCzasDelta(DWORD, DWORD);
#endif
};

#endif
