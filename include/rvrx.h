#ifndef _RVRX_H
#define _RVRX_H

/* Klasa pomocnicza dla dostępu do plików (definicje stałych
* i nagłówków) i strumieni TCP */

#include "typy.h"


/* Ustawienia specyficzne dla Linuxa */
#ifdef PLATFORM_LINUX

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define ODCZYT_BINARNY (O_RDONLY | PLIK_DUZY)
#define ZAPIS_BINARNY (O_WRONLY | O_CREAT | O_TRUNC | PLIK_DUZY)
#define PRAWA_ZAPISU (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH)
#define BLAD_CONN_RESET ECONNRESET
#endif

/* Ustawienia specyficzne dla Windows */
#ifdef PLATFORM_WIN
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <winsock2.h>
#include <windows.h>

#define ODCZYT_BINARNY (O_RDONLY | O_BINARY)
#define ZAPIS_BINARNY (O_WRONLY | O_CREAT | O_TRUNC | O_BINARY)
#define PRAWA_ZAPISU (S_IREAD | S_IWRITE)
#define BLAD_CONN_RESET WSAECONNRESET
#endif

#if AKTYWNY_RIVER

class River
{
    int aktywna; /* Czy sieć jest aktywna i można z niej korzystać */
    #if PLATFORM_WIN
    WSADATA wsd;
    #endif
    struct sockaddr_in sin; /* Opis gniazda TCP */

    public:
    River(void);
    int Init(void);
    void Finish(void);
    int OpenDaemonTCP(char *);
    int OpenClientTCP(char *, char *);
    int SendMultiTCP(int, Byte *, int);
    int RecvMultiTCP(int, Byte *, int, int *);
    int InitStruct(struct sockaddr_in *);
    int FillAddress(struct sockaddr_in *, char *);
    int FillPort(struct sockaddr_in *, char *);
    #if PLATFORM_LINUX
    void CloseSocketTCP(int);
    #endif
    #if PLATFORM_WIN
    void CloseSocketTCP(SOCKET);
    #endif
};

extern River TopRiver; /* Informacja dla chcących skorzystać z klasy */

#endif
#endif
