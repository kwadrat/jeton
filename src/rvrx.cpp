#include "rvrx.h"

#if AKTYWNY_RIVER

#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* Główny i jedyny obiekt tej klasy. Wszystkie odwołania powinny być do niego
* kierowane, jeśli chcemy robić coś z siecią TCP. */
River TopRiver;

River::River(void)
{
    aktywna = 0; /* Jeszcze nikt nie chciał pracować w sieci TCP */
}

/* Przygotowuje sieć do działania. Funkcja może być wywoływana wielokrotnie,
* ale zadziała tylko raz, za pierwszym wywołaniem. Wartość zwrotna: 1 - OK,
* 0 - wystąpił błąd */
int River::Init(void)
{
    int status;
    status = 0;
    if(aktywna == 0)
    {
        #ifdef PLATFORM_WIN
        if(WSAStartup(MAKEWORD(1,1), &wsd) == 0)
        {
            status = 1;
        }
        else
        {
            SygError("Nie można wczytać biblioteki Winsock 1.1");
        }
        #endif
        aktywna = 1;
    }
    return status;
}

void River::Finish(void)
{
    if(aktywna)
    {
        #ifdef PLATFORM_WIN
        WSACleanup();
        #endif
        aktywna = 0;
    }
}

/* Zaczyna nasłuchiwać na podanym porcie nadchodzącego połączenia TCP. Zwracany
* jest numer socketu lub -1 dla poinformowania o błędnym zadziałaniu. */
int River::OpenDaemonTCP(char * PortName)
{
    int status;
    int s_odt;
    int s_bnd;
    int tmplen;
    int plik;
    status = -1;
    Init();
    s_odt = InitStruct( & sin);
    if(s_odt != -1)
    {
        #if AKTYWNY_REUSEADDR
        /* qaz - Trzeba się zastanowić, co w ogóle daje ta opcja. Jeśli to będzie
        * wiadome, to w tym miejscu należy to odpowiednio skomentować. */
        tmplen = 1;
        setsockopt(s_odt, SOL_SOCKET, SO_REUSEADDR,
        #ifdef PLATFORM_WIN
        (char *)
        #endif
        & tmplen, sizeof(tmplen));
        #endif
        if(TopRiver.FillPort( & sin, PortName))
        {
            s_bnd = bind(s_odt, (struct sockaddr *) & sin, sizeof(sin));
            if(s_bnd != -1)
            {
                listen(s_odt, 3); /* Tu poczekamy na zgłoszenie */
                #ifdef PLATFORM_WIN
                #if 0
                printf("Po listen() WSAGetLastError = %d\n", WSAGetLastError());
                #endif
                #endif
                tmplen = sizeof(sin);
                plik = accept(s_odt,
                /* #ifdef PLATFORM_LINUX qaz */
                #if 1
                (struct sockaddr *)
                #endif
                & sin,
                #ifdef PLATFORM_LINUX
                (unsigned int *)
                #endif
                & tmplen);
                #ifdef PLATFORM_WIN
                #if 0
                printf("Po accept() WSAGetLastError = %d\n", WSAGetLastError());
                #endif
                #endif
                if(plik != -1)
                {
                    status = plik;
                }
                else
                {
                    SygErrorParm("accept() failed, errno = %d\n", errno);
                }
            }
            else
            {
                SygErrorParm("bind() failed, errno = %d\n", errno);
            }
        }
        else
        {
            SygError("Nie udało się ustawić numeru portu");
        }
    }
    else
    {
        SygError("Nie powiodło się wywołanie InitStruct");
    }
    return status;
}

/* Podłącza się do podanego hosta na podany numer portu TCP. Zwracany jest
* numer socketu lub -1 dla poinformowania o błędnym zadziałaniu. */
int River::OpenClientTCP(char * HostName, char * PortName)
{
    int status;
    int s_oct;
    int plik;

    status = -1;
    Init();
    plik = InitStruct( & sin);
    if(plik != -1)
    {
        if(TopRiver.FillAddress( & sin, HostName))
        {
            if(TopRiver.FillPort( & sin, PortName))
            {
                s_oct = connect(plik, (struct sockaddr *) & sin, sizeof(sin));
                if(s_oct >= 0)
                {
                    status = plik;
                }
                else
                {
                    SygErrorParm("connect() zwrócił errno = %d", errno);
                }
            }
            else
            {
                SygError("Nie udało się ustawić numeru portu");
            }
        }
        else
        {
            SygError("Nie udało się ustawić numeru portu");
        }
    }
    else
    {
        SygErrorParm("Funkcja open() zwróciła %d\n", errno);
    }
    return status;
}

/* Wartość zwrotna: liczba wysłanych bajtów. Zakładamy, że funkcja będzie
* wywoływana tylko z dodatnią liczbą bajtów do wysłania. Wartość zwrotna może
* być ujemna, co sugeruje błąd w czasie transmisji. */
int River::SendMultiTCP(int s_smt, Byte * poczatek, int ile)
{
    int status;
    status = -1;
    int suma;
    int kes;
    suma = 0;
    while(1)
    {
        if(suma == ile)
        {
            status = suma;
            break;
        }
        kes = send(s_smt,
        #ifdef PLATFORM_WIN
        (char *)
        #endif
        poczatek + suma, ile - suma, 0);
        if(kes > 0)
        {
            suma += kes;
        }
        else
        {
            break; /* Błąd - zwracamy "-1" */
        }
    }
    return status;
}

/* Wartość zwrotna: liczba odebranych bajtów. Zakładamy, że funkcja może być
* wywoływana tylko z dodatnim licznikiem bajtów. Jeśli skończyły się dane, to
* zwracamy 0. Jeśli przy jakimś odczycie zostało zwrócone zero (End of File),
* to ustawiamy flagę "eof_flag" */

int River::RecvMultiTCP(int s_rmt, Byte * poczatek, int ile, int * eof_flag)
{
    int status;
    int suma;
    int kes;
    status = -1;
    suma = 0;
    while(1)
    {
        if(suma == ile)
        {
            status = suma;
            break;
        }
        kes = recv(s_rmt,
        #ifdef PLATFORM_WIN
        (char *)
        #endif
        poczatek + suma, ile - suma, 0);
        if(kes > 0)
        {
            suma += kes;
        }
        else
        {
            if(kes == 0)
            {
                status = suma;
                *eof_flag = 1;
                break;
            }
            else
            {
                if(errno == BLAD_CONN_RESET)
                {
                    /* Druga strona po prostu zamknęła połączenie */
                    status = suma; /* Nie odebraliśmy żadnych danych */
                    *eof_flag = 1;
                    break;
                }
                else
                {
                    break; /* Błąd - zwracamy "-1" */
                }
            }
        }
    }
    return status;
}

/* Zwraca identyfikator gniazdka (jeśli ujemny, to wystąpił błąd) */
int River::InitStruct(struct sockaddr_in * sin)
{
    int s_is; /* Socket */
    memset(sin, 0, sizeof(struct sockaddr_in));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = INADDR_ANY;
    s_is = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    return s_is;
}

/* W podanej strukturze wypełnia pole adresu IP. Wartość zwrotna: 1 - OK,
* 0 - błąd */
int River::FillAddress(struct sockaddr_in * sin, char * napis)
{
    int status;
    struct hostent * host;
    status = 0;
    sin->sin_addr.s_addr = inet_addr(napis);
    if(sin->sin_addr.s_addr != INADDR_NONE)
    {
        status = 1;
    }
    else
    {
        #if NET_SIMPLE
        host = numeric_ip_to_implement(napis);
        #else // NET_SIMPLE
        host = gethostbyname(napis);
        #endif // NET_SIMPLE
        if(host != NULL)
        {
            memcpy( & (sin->sin_addr.s_addr), host->h_addr_list[0], host->h_length);
            status = 1;
        }
        else
        {
            SygErrorParm("Nie udało się uzyskać numeru IP dla \"%s\"", napis);
        }
    }
    return status;
}

/* W podanej strukturze wypełnia pole portu IP. Wartość zwrotna: 1 - OK,
* 0 - błąd */
int River::FillPort(struct sockaddr_in * sin, char * napis)
{
    int status;
    struct servent * pse;
    status = 0;
    #if NET_SIMPLE
    pse = numeric_port_to_implement(napis);
    #else // NET_SIMPLE
    pse = getservbyname(napis, "tcp");
    #endif // NET_SIMPLE
    if(pse != NULL)
    {
        sin->sin_port = pse->s_port;
        status = 1;
    }
    else
    {
        sin->sin_port = htons( (u_short) atoi(napis));
        if(sin->sin_port > 0)
        {
            status = 1;
        }
        else
        {
            SygErrorParm("Numer portu nie jest dodatni: %d", sin->sin_port);
        }
    }
    return status;
}

#if PLATFORM_LINUX
void River::CloseSocketTCP(int s_cst)
{
    close(s_cst);
}
#endif

#if PLATFORM_WIN
void River::CloseSocketTCP(SOCKET s_cst)
{
    closesocket(s_cst);
}
#endif

#endif
