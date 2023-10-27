#include "masterx.h"

MasterClass::MasterClass(void)
{
    HeadPtr = NULL;
}

MasterClass::~MasterClass(void)
{
}

void MasterClass::LocalDestructor(void)
{
    if(HeadPtr != NULL)
    {
        HeadPtr->LocalDestructor();
        delete HeadPtr;
        HeadPtr = NULL;
    }
}

/* Dopisuje do łańcucha procesów nowy proces.
* Wartość zwrotna: 1 - OK, 0 - błąd */
int MasterClass::AddProcess(ProcessClass * proc)
{
    int status;
    ProcessClass * tmp;

    status = 0;
    if(HeadPtr == NULL)
    {
        if(proc->ProcesWejsciowy())
        {
            HeadPtr = proc;
            status = 1;
        }
        else
        {
            SygError("Pierwszy podany obiekt nie może pracować jako obiekt wejściowy");
        }
    }
    else
    {
        tmp = HeadPtr->FindUnusedPointer();
        if(tmp != NULL)
        {
            if(tmp->ZgodnyTyp(proc))
            {
                status = tmp->ConnectPointer(proc);
            }
            else
            {
                SygError("Niezgodność typu obiektu z poprzednim obiektem.");
                status = 0;
            }
        }
        else
        {
            SygError("Nie znaleziono miejsca w łańcuchu, gdzie można dopisać element");
        }
    }
    return status;
}

/* Uruchamia cały łańcuch zdefiniowanych procedur */
void MasterClass::RunProcesses(void)
{
    if(HeadPtr != NULL)
    {
        HeadPtr->Rozszczepianie();
        HeadPtr->Work(SAND_GENERAL, NULL, KIER_PROSTO);
    }
    else
    {
        SygError("Brak procesów w ciągu.");
    }
}

/* Sprawdza, czy łańcuch jest kompletny. Wartość zwrotna: 1 - łańcuch jest
* kompletny (nic się już nie da do niego dodać), 0 - do łańcucha wciąż można
* dodać pewne elementy, a to znaczy, że użytkownik nie podał prawidłowego
* ciągu elementów, co skutkowałoby awaryjnym działaniem aplikacji. */
int MasterClass::CheckComplete(void)
{
    int status;
    ProcessClass * tmp;

    status = 0;
    if(HeadPtr != NULL)
    {
        tmp = HeadPtr->FindUnusedPointer();
        if(tmp == NULL)
        {
            status = 1;
        }
    }
    else
    {
        SygError("Brak procesów w ciągu.");
    }
    return status;
}
