#include "semx.h"
#include "typy.h"

#include <assert.h>
#include <stdlib.h>

#if SEMAFOR_JEDNOPOZIOMOWY
#else

SimpleSem::SimpleSem(void)
{
    #ifdef PLATFORM_WIN
    TworzenieOK = 0; /* Na razie jeszcze nie jest przygotowany semafor */
    hSemaphore = CreateSemaphore(
        NULL, /* no security attributes */
        1, /* wartość początkowa */
        1, /* wartość maksymalna */
        NULL /* semafor nie będzie miał nazwy */
        );
    if(hSemaphore != NULL)
    {
        TworzenieOK = 1;
    }
    else
    {
        SygErrorParm("CreateSemaphore zwróciło %d", GetLastError());
    }
    #endif
    #ifdef PLATFORM_LINUX
    int status;
    TworzenieOK = 0; /* Na razie jeszcze nie jest przygotowany semafor */
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(semid >= 0)
    {
        sem_ds.val = 1; /* Domyślnie wpuścimy jednego klienta */
        status = semctl(semid, 0, SETVAL, sem_ds);
        if(status >= 0)
        {
            TworzenieOK = 1;
        }
        else
        {
            SygErrorParm("Funkcja semctl(SETVAL) zwróciła %d\n", errno);
        }
    }
    else
    {
        SygErrorParm("Funkcja semget() zwróciła %d\n", errno);
    }
    #endif
}

SimpleSem::~SimpleSem(void)
{
    /* Tu nie powinniśmy dojść pod Windows, bo Linux jakoś nie potrafi tu dojść */
    assert(1 == 0);
}

void SimpleSem::LocalDestructor(void)
{
    #ifdef PLATFORM_WIN
    if(TworzenieOK)
    {
        if(hSemaphore != NULL)
        {
            CloseHandle(hSemaphore);
            hSemaphore = NULL;
        }
    }
    #endif
    #ifdef PLATFORM_LINUX
    int status;

    status = semctl(semid, 1, IPC_RMID, sem_ds);
    if(status < 0)
    {
        SygErrorParm("Funkcja sysctl(IPC_RMID) zwróciła %d\n", errno);
    }
    #endif
}

void SimpleSem::P(void)
{
    #ifdef PLATFORM_WIN
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(hSemaphore, INFINITE);
    if(dwWaitResult != WAIT_OBJECT_0)
    {
        SygErrorParm("WaitForSingleObject() zwrócił %d\n", dwWaitResult);
        exit(0);
    }
    #endif
    #ifdef PLATFORM_LINUX
    struct sembuf semb;
    int status;
    semb.sem_num = 0; /* Identyfikator semafora w grupie semaforów */
    semb.sem_op = -1; /* Blokujemy */
    semb.sem_flg = 0;

    status = semop(semid, &semb, 1);
    if(status < 0)
    {
        SygErrorParm("Funkcja semop(-1) zwróciła %d\n", errno);
    }
    #endif
}

void SimpleSem::V(void)
{
    #ifdef PLATFORM_WIN
    BOOL kod;
    kod = ReleaseSemaphore(hSemaphore, 1, NULL);
    if( ! kod)
    {
        SygErrorParm("ReleaseSemaphore() zwrócił %d\n", GetLastError());
        exit(0);
    }
    #endif
    #ifdef PLATFORM_LINUX
    struct sembuf semb;
    int status;
    semb.sem_num = 0; /* Identyfikator semafora w grupie semaforów */
    semb.sem_op = 1; /* Zwalniamy  */
    semb.sem_flg = 0;

    status = semop(semid, &semb, 1);
    if(status < 0)
    {
        SygErrorParm("Funkcja semop(-1) zwróciła %d\n", errno);
    }
    #endif
}

/* Informuje, że semafor został poprawnie utworzony (1) lub był błąd (0).
* Wstępnie semafor jest tworzony w trybie otwartym, wpuszczającym jednego
* klienta. */
int SimpleSem::OK(void)
{
    return TworzenieOK;
}

#endif

Semafor::Semafor(void)
{
    AktStan = 0;
    #if SEMAFOR_JEDNOPOZIOMOWY
    TworzenieOK = 1;
    #else
    TworzenieOK = 0;
    if(SesamSem.OK())
    {
        if(ValueSem.OK())
        {
            TworzenieOK = 1;
        }
        else
        {
            SygError("Błąd przy tworzeniu ValueSem");
        }
    }
    else
    {
        SygError("Błąd przy tworzeniu SesamSem");
    }
    #endif
}

Semafor::~Semafor(void)
{
    /* Tu za bardzo nie ma pomysłu, co by robić */
}

void Semafor::LocalDestructor(void)
{
    #if SEMAFOR_JEDNOPOZIOMOWY
    #ifdef PLATFORM_WIN
    if(TworzenieOK)
    {
        if(hSemaphore != NULL)
        {
            CloseHandle(hSemaphore);
            hSemaphore = NULL;
        }
    }
    #endif
    #ifdef PLATFORM_LINUX
    int status;

    status = semctl(semid, 1, IPC_RMID, sem_ds);
    if(status < 0)
    {
        SygErrorParm("Funkcja sysctl(IPC_RMID) zwróciła %d\n", errno);
    }
    #endif
    #else
    SesamSem.LocalDestructor();
    ValueSem.LocalDestructor();
    #endif
}

/* Wymusza ustawienie semafora. Jeśli wartość jest poniżej 1, to następuje
aktywacja blokady. Ta funkcja musi być wywoływana, aby semafor mógł pracować
normalnie. Wartość zwrotna: 1 - OK, 0 - błąd */
int Semafor::UstawWartosc(int value, int ilest)
{
    MinVal = value;
    #if SEMAFOR_JEDNOPOZIOMOWY
    #ifdef PLATFORM_WIN
    TworzenieOK = 0; /* Na razie jeszcze nie jest przygotowany semafor */
    hSemaphore = CreateSemaphore(
      NULL, /* no security attributes */
      value, /* wartość początkowa */
      ilest, /* wartość maksymalna */
      NULL /* semafor nie będzie miał nazwy */
      );
    if(hSemaphore != NULL)
    {
        TworzenieOK = 1;
    }
    else
    {
        SygErrorParm("CreateSemaphore zwróciło %d", GetLastError());
    }
    #endif
    #ifdef PLATFORM_LINUX
    int status;
    TworzenieOK = 0; /* Na razie jeszcze nie jest przygotowany semafor */
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(semid >= 0)
    {
        sem_ds.val = value; /* Domyślnie wpuścimy jednego klienta */
        status = semctl(semid, 0, SETVAL, sem_ds);
        if(status >= 0)
        {
            TworzenieOK = 1;
        }
        else
        {
            SygErrorParm("Funkcja semctl(SETVAL) zwróciła %d\n", errno);
        }
    }
    else
    {
        SygErrorParm("Funkcja semget() zwróciła %d\n", errno);
    }
    #endif
    #else
    SesamSem.P();
    Value = value;
    if(Value <= 0)
    {
        ValueSem.P();
    }
    SesamSem.V();
    #endif
    LiczbaStanow = ilest; /* Liczba możliwych stanów semafora */
    return TworzenieOK;
}

/* Zwraca dla semafora DostMiejsce liczbę buforów, które zostały wykorzystane
*/
int Semafor::IleWykorzystano(void)
{
    int ile;
    /* Nie jestem pewien, czy te semafory są konieczne, gdyż tylko chcemy
    odczytać te dane */
    #if SEMAFOR_JEDNOPOZIOMOWY
    ile = LiczbaStanow - MinVal;
    #else
    SesamSem.P();
    ile = LiczbaStanow - MinVal;
    SesamSem.V();
    #endif
    return ile;
}

int Semafor::SemP(void)
{
    int status;
    #ifdef PLATFORM_LINUX
    int tmpval; /* Tymczasowa wartość semafora */
    #endif
    status = AktStan;
    AktStan++;
    if(AktStan == LiczbaStanow)
    {
        AktStan = 0; /* Zawijamy licznik okrężny */
    }
    #if SEMAFOR_JEDNOPOZIOMOWY
    #ifdef PLATFORM_WIN
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(hSemaphore, INFINITE);
    if(dwWaitResult != WAIT_OBJECT_0)
    {
        SygErrorParm("WaitForSingleObject() zwrócił %d\n", dwWaitResult);
        exit(0);
    }
    #endif
    #ifdef PLATFORM_LINUX
    struct sembuf semb;
    int s;
    semb.sem_num = 0; /* Identyfikator semafora w grupie semaforów */
    semb.sem_op = -1; /* Blokujemy */
    semb.sem_flg = 0;

    s = semop(semid, &semb, 1);
    if(s < 0)
    {
        SygErrorParm("Funkcja semop(-1) zwróciła %d\n", errno);
        exit(0);
    }
    tmpval = semctl(semid, 0, GETVAL, sem_ds);
    if(tmpval == -1)
    {
        SygErrorParm("Funkcja semctl_GETVAL(-1) zwróciła %d\n", errno);
        exit(0);
    }
    if(MinVal > tmpval)
    {
        MinVal = tmpval;
    }
    #endif
    #else
    SesamSem.P();
    Value --;
    if(Value <= 0)
    {
        MinVal = 0;
        SesamSem.V(); /* Ważna kolejność - najpierw zwalniamy blokadę */
        ValueSem.P();
    }
    else
    {
        if(MinVal >= Value)
        {
            MinVal = Value; /* To może najwyżej spaść do zera */
        }
        SesamSem.V();
    }
    #endif
    return status;
}

/* Zwraca na pewno stan wcześniejszy licznika semafora, niż ten, jaki będzie
po wykonaniu operacji zwalniania bufora */
int Semafor::SemV(void)
{
    int status;
    status = AktStan;
    #if SEMAFOR_JEDNOPOZIOMOWY
    #ifdef PLATFORM_WIN
    int tmpval; /* Tymczasowa wartość semafora */
    BOOL kod;
    kod = ReleaseSemaphore(hSemaphore, 1, (LPLONG) & tmpval);
    if( ! kod)
    {
        SygErrorParm("ReleaseSemaphore() zwrócił %d\n", GetLastError());
        exit(0);
    }
    if(MinVal > tmpval)
    {
        MinVal = tmpval;
    }
    #endif
    #ifdef PLATFORM_LINUX
    struct sembuf semb;
    int s;
    semb.sem_num = 0; /* Identyfikator semafora w grupie semaforów */
    semb.sem_op = 1; /* Zwalniamy  */
    semb.sem_flg = 0;

    s = semop(semid, &semb, 1);
    if(s < 0)
    {
        SygErrorParm("Funkcja semop(-1) zwróciła %d\n", errno);
    }
    #endif
    #else
    SesamSem.P();
    Value ++;
    if(Value <= 1)
    {
        ValueSem.V();
        SesamSem.V();
    }
    else
    {
        SesamSem.V();
    }
    #endif
    return status;
}
