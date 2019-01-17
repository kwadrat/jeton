#ifndef _SEMX_H
#define _SEMX_H

#include "typy.h"
#include "rvrx.h"


#ifdef PLATFORM_LINUX
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/* Poniższy blok pochodzi z "man semctl" */
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif

#endif

#if SEMAFOR_JEDNOPOZIOMOWY
#else

class SimpleSem
{
#ifdef PLATFORM_WIN
 HANDLE hSemaphore;
#endif
#ifdef PLATFORM_LINUX
 int semid;
 semun sem_ds;
#endif
 int TworzenieOK;

public:
 SimpleSem(void); /* Semafor jest tworzony w stanie +1 (wpuszczę jednego) */
 ~SimpleSem(void);
 void LocalDestructor(void);
 void P(void);
 void V(void);
 int OK(void);
};

#endif

class Semafor
{
#if SEMAFOR_JEDNOPOZIOMOWY
#ifdef PLATFORM_WIN
 HANDLE hSemaphore;
#endif
#ifdef PLATFORM_LINUX
 int semid;
 semun sem_ds;
#endif
#else
 SimpleSem SesamSem; /* Pilnuje wejścia do procedury */
 SimpleSem ValueSem; /* Powoduje blokadę w momencie zmiany wartości semafora */
 int Value; /* Aktualna wartość semafora wielowartościowego */
#endif
 int MinVal; /* Najniższa zarejestrowana wartość semafora */
 int LiczbaStanow; /* Liczba możliwych stanów - aby zbudować licznik okrężny */
 int AktStan; /* Aktualnie pierwszy stan do wzięcia - licznik okrężny */
 int TworzenieOK;
public:
 Semafor(void);
 ~Semafor(void);
 void LocalDestructor(void);
 int UstawWartosc(int, int);
 int IleWykorzystano(void);
 int SemP(void);
 int SemV(void);
};
#endif
