#ifndef _FSMAR_H
#define _FSMAR_H

#include "typy.h"

#if AKTYWNY_FSMAR


#include "procx.h"
#include "semx.h"

#ifdef PLATFORM_LINUX
#include <pthread.h>
#endif

#ifdef PLATFORM_WIN
#endif

/* Status bufora o podanym numerze */
#define F_SMAR_STATUS(i) (((int *) Dane)[(i)])

/* Początek bufora o podanym numerze */
#define F_SMAR_BUF_START(i)  (Dane + ROZM_INT * LiczbaBuf + (i) * RozmBuf)

class smar_Class : public ProcessClass
{
 Byte * Dane; /* Lewy+Prawy */
 Semafor DostDane; /* Dostępne bufory z danymi */
 Semafor DostMiejsce; /* Dostępne bufory z miejscem */
 int AktLewyBuf; /* Lewy */
 int AktPrawyBuf; /* Prawy */
 int LewyMiejsce; /* Lewy - jeśli 0, to znaczy, że nie ma bufora */
 int LewyZajete; /* Lewy */
 int PrawyMiejsce; /* Prawy - jeśli 0, to znaczy, że nie ma bufora */
 int PrawyZajete; /* Prawy */
 int PrawyPobrane; /* Prawy */
 int LiczbaBuf; /* Lewy+Prawy */
 int RozmBuf; /* Lewy+Prawy */
 int ShadowState; /* Prawy, stan drugiego procesu */
 int AdvancedInit(int, int);
 /* Flaga, czy na końcu wyświetlić maksymalną liczbę zajętych buforów */
 int PokazujWykorzystanie;
 int IleJestWykorzystane;

#ifdef PLATFORM_LINUX
 pthread_t thread; /* Lewy */
#endif

#ifdef PLATFORM_WIN
  HANDLE hThread; /* Uchwyt do lewego procesu */
  DWORD dwThreadParam; /* Parametr wywołania programu */
  DWORD dwThreadId; /* Identyfikator wątku, ignorowany */
#endif

public:
 virtual int Init(int, char *[]);
 virtual int Work(int, Byte *, int);
 virtual void LocalDestructor(void);
 virtual void ZmienStan(int, int);
 virtual int Rozszczepianie(void);
 smar_Class(void);
 int Obsluga_SAND_GENERAL(int, Byte *, int);
 int Obsluga_SAND_OFF(int, Byte *, int);
 int Obsluga_SAND_GENERAL_Dla_Lewego(void);
 int Obsluga_SAND_GENERAL_Dla_Prawego(void);
 int FunkcjaWatka(void);
 int DodajWatek(void);
 void PoczekajNaWatek(void);
};

#endif
#endif
