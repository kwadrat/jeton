#ifndef _TYPY_H
#define _TYPY_H

/* Rozpoznawanie systemu operacyjnego */
#ifdef WIN32
#define PLATFORM_WIN 1
#else
# ifdef linux
#define PLATFORM_LINUX 1
# else
#  include "error! Nieznany komputer"
# endif
#endif

/* Poniżej wykrywamy, czy glibc jest w stanie obsłużyć pliki ponad 2 GB. Nie
 * można tego zrobić przez sprawdzenie wartości stałej "__GLIBC__", bo jest ona
 * definiowana w pliku "features.h", który jednocześnie włącza 64-bitową
 * obsługę plików.
 */
#if PLATFORM_LINUX
# define _LARGEFILE64_SOURCE 1
# undef __USE_LARGEFILE64
# include <features.h>
# ifdef __USE_LARGEFILE64
  /* Tu definiujemy flagę, że pliki mogą być większe niż 2 GB */
#  define PLIK_F_DUZY 1
# else
#  define PLIK_F_DUZY 0
# endif

# if PLIK_F_DUZY
#  define PLIK_DUZY O_LARGEFILE
# else
#  define PLIK_DUZY 0
# endif
/* Ten kod nie jest już używany do włączania obsługi wielkich plików */
#if 0
#  define _FILE_OFFSET_BITS 64
#  include <sys/types.h>
#endif
#endif


#include <stdio.h>

/* Konfiguracja działania programu */

/* UWAGA: Ta zmienna jeszcze nie jest nigdzie wykorzystywana. */
/* Zmienia sposób działania procesów: 1 - przekazywane bloki są większe, gdyż
 * przy zapisie do skutku jest powtarzana operacja zapisu, aż zostaną wysłane
 * wszystkie dane. 0 - każdy element akceptuje taki rozmiar przesłanego bloku,
 * jaki otrzymał od sąsiedniego procesu, a to oznacza, że bloki będą krótsze i
 * częściej będzie się odbywać przełączanie między procesami */
#define GRUPOWANIE_DANYCH 1


/* Włączanie i wyłączanie poszczególnych modułów programu */
#define AKTYWNY_FSMAR 1
#define AKTYWNY_FCAT 1
/* Kod dla struktury RIVER i pochodnych: toc, tos, tic, tis */
#define AKTYWNY_RIVER 1
/* qaz - Nie wiadomo, po co poniżej jest 1, albo po co miałoby być 0 */
#define AKTYWNY_REUSEADDR 1
#define AKTYWNY_MD5 1

/* Styl klasyczny - na zakończenie transmisji informuje, ile przesłano bajtów i
 * w jakim czasie. Styl zaawansowany: potrzebuje jako parametru liczby sekund,
 * co ile ma pokazywać aktualne osiągi. Styl klasyczny można będzie w
 * przyszłości wymuszać przez podanie "0" jako parametru. */
#define ACCEPTOR_STYLE_CLASSIC 1


/* Semafor uproszczony, jednopoziomowy, wielowartościwy (najpierw był
 * zaimplementowany semafor dwupoziomowy w oparciu o semafory binarne,
 * bo pierwotna koncepcja zakładała wykorzystanie binarnych rejonów
 * krytycznych w Win95) */
/* Semafor rozbudowany, wielopoziomowy za to potrafi oszacować liczbę
 * wykorzystanych buforów w czasie transmisji */

#define SEMAFOR_JEDNOPOZIOMOWY 1
/* Wielkość zmiennej typu "int" */
#define ROZM_INT 4




#define DIAGNOSTIC 0



/*  Typy komunikatów wymienianych między procesami - pierwszy parametr
 *  funkcji Work() */
/* Samo przekazanie sterowania do procesu */
#define SAND_GENERAL (-4)
/* Data Request - żądanie przysłania trochę danych (początek, ile) */
#define SAND_DR (-5)
/* Space Request - żądanie zwolnienia miejsca, czyli oferta udostępnienia danych (początek, ile) */
#define SAND_SR (-6)
/* Zakładamy, że powyższy komunikat może nie będzie potrzebny w systemie */
#if 1
/* EOF - sygnalizacja, że nie będzie więcej danych do pobrania */
#define SAND_EOF (-7)
#endif
/* OFF - sygnalizacja, że system ma się wyłączyć */
#define SAND_OFF (-8)

#define SygError(x) fprintf(stderr, "!!! Plik \"%s\" Linia %d, \"%s\"\n",\
                            __FILE__, __LINE__, (x))

#define SygErrorParm(x,y) \
{ \
 char Komunikat[1000]; \
 sprintf(Komunikat, (x), (y)); \
 SygError(Komunikat); \
}


#if 1
#define DIAGNOZAA 0
#define DIAGNA(x) {if(DIAGNOZAA){fprintf(stderr,(x));} }
#define DIAGNA2(x,y) {if(DIAGNOZAA){fprintf(stderr,(x),(y));} }
#define DIAGNA3(x,y,z) {if(DIAGNOZAA){fprintf(stderr,(x),(y),(z));} }
#define DIAGNA4(x,y,z,w) {if(DIAGNOZAA){fprintf(stderr,(x),(y),(z),(w));} }
#endif

typedef unsigned char Byte;


/* Wartość zwracana, gdy w systemie pojawił się taki błąd, że nie da się
 * kontynuować pracy. */
#define RESULT_OFF (-9)
/* Ta wartość może zostać zwrócona w odpowiedzi na SAND_DR, informuje, że na
 * razie nie ma żadnych danych do wzięcia i najlepiej, jak proces się skończy,
 * aby można mu było przygotować następną porcję danych. */
#define RESULT_AGAIN (-10)
/* Wartość zwracana przez proces, który stwierdza, że już więcej nie wygeneruje
 * danych do pobrania. Odebranie takiej zmiennej powinno skutkować zmianą stanu
 * naszego procesu na STATE_EOF */
#define RESULT_EOF (-11)

/* Poniższe stany powinny być liczbami ujemnymi, aby mogły być zwrócone przez
 * funkcję Work(). Te liczby są także wykorzystywane przez zmienną State dla
 * danego procesu, aby on sam wiedział, w jakim jest stanie. */
/* Określa, że proces może czytać (żądać danych) od poprzedniego procesu. */
#define STATE_ON (-12)
/* W takim stanie proces już nie może żądać danych od poprzedniego procesu, bo
 * danych brakło. */
#define STATE_EOF (-13)
/* Taki stan przyjmuje zadanie, które stwierdza, że dłużej się nie da pracować.
 * Po przyjęciu tego stanu wysyła jeszcze komunikaty do sąsiednich elementów i
 * następnie kończy obsługę jakichkolwiek komunikatów. */
#define STATE_OFF (-14)

/* Stała identyfikująca proces zapisujący do bufora */
#define SMAR_PRODUCEN (-15)
/* Stała identyfikująca proces czytający z bufora */
#define SMAR_KONSUMENT (-16)

/* Przekazywanie zmiany stanu z lewej do prawej */
#define KIER_PROSTO (-17)
/* Przekazywanie zmiany stanu z prawej do lewej */
#define KIER_WSTECZ (-18)
/* Nieznany kierunek zmian, powiadom obie strony */
#define KIER_INNY (-19)

#endif
