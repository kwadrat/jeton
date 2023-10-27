#include "procx.h"

#include <assert.h>

ProcessClass::ProcessClass(void)
{
    State = STATE_ON;
    RodzajWej = '.';
    RodzajWyj = '.';
    PrevProcess = NULL;
    NextProcess = NULL;
}

void ProcessClass::LocalDestructor(void)
{
    if(NextProcess != NULL)
    {
        NextProcess->LocalDestructor();
        delete NextProcess;
        NextProcess = NULL;
    }
}

ProcessClass::~ProcessClass(void)
{
    LocalDestructor();
}

int ProcessClass::Init(int, char *[])
{
    SygError("To nie powinno być wywoływane");
    return RESULT_OFF;
}

int ProcessClass::Work(int, Byte *, int)
{
    SygError("To nie powinno być wywoływane");
    return 0;
}

ProcessClass * ProcessClass::FindUnusedPointer(void)
{
    ProcessClass * tmp;

    tmp = NULL;
    if(NextProcess != NULL)
    {
        tmp = NextProcess->FindUnusedPointer();
    }
    else
    {
        if(RodzajWyj == 'M' || RodzajWyj == 'B')
        {
            tmp = this;
        }
    }
    return tmp;
}

/* Podłącza w wolne miejsce wskaźnik następnego procesu. Wartość zwracana:
1 - OK, 0 - błąd */
int ProcessClass::ConnectPointer(ProcessClass * proc)
{
    int status;
    status = 0;
    if(NextProcess == NULL)
    {
        NextProcess = proc;
        proc->PrevProcess = this;
        status = 1;
    }
    else
    {
        SygError("Proces twierdził, że ma miejsce na podłączenie następnego?");
    }
    return status;
}

/* Sprawdza, czy do aktualnego obiektu można podłączyć następny obiekt.
Dozwolone są tylko połączenia "B->M" i "M->B". Wartość zwrotna: 1 - typy
są zgodne, 0 - typy są niezgodne, co zabrania tworzenia takiego połączenia */
int ProcessClass::ZgodnyTyp(ProcessClass *proc)
{
    int status;
    status = 0;
    if(RodzajWyj == 'B')
    {
        if(proc->RodzajWej == 'M')
        {
            status = 1;
        }
    }
    else
    {
        if(RodzajWyj == 'M')
        {
            if(proc->RodzajWej == 'B')
            {
                status = 1;
            }
        }
    }
    return status;
}

/* Funkcja sprawdza, czy podany proces może pracować jako proces wejściowy.
Wartość zwrotna: 1 - tak, 0 - nie */
int ProcessClass::ProcesWejsciowy(void)
{
    int status;
    status = 0;
    if(RodzajWej == '-')
    {
        status = 1;
    }
    return status;
}

/* Zmienia stan obiektu na podany. Jeśli to jest zmiana na STATE_OFF,
to od razu powiadamia sąsiadów z obu stron. */
void ProcessClass::ZmienStan(int nowy, int kier)
{
    assert(nowy == STATE_OFF || nowy == STATE_EOF);
    assert(kier == KIER_PROSTO || kier == KIER_WSTECZ || kier == KIER_INNY);
    if(nowy == STATE_OFF)
    {
        if(State != nowy)
        {
            State = nowy;
            if(kier != KIER_WSTECZ)
            {
                if(NextProcess != NULL)
                {
                    NextProcess->Work(SAND_OFF, NULL, KIER_PROSTO);
                }
            }
        }
    }
    else
    {
        State = nowy;
    }
}

/* Tworzenie nowych wątków dla obsługi systemu i budzenie ich.
Wartość zwrotna: 1 - OK (udało się pomyślnie utworzyć nowe wątki),
0 - błąd (możemy kończyć pracę systemu, bo się nie udał przydział wątków */
int ProcessClass::Rozszczepianie(void)
{
    int status;
    status = 1; /* Tak ogólnie niczego nie trzeba rozszczepiać */
    if(NextProcess != NULL)
    {
        status = NextProcess->Rozszczepianie();
    }
    return status;
}
