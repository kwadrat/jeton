#include "facptr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEP_BF_SIZE 28 /* Bufor na 64-bitową liczbę, która ma 26 znaków, gdy tysiące są separowane spacjami */

acptr_Class::acptr_Class(void)
{
 RodzajWej = 'B';
 RodzajWyj = '-';
 RTab = NULL;
}

void acptr_Class::LocalDestructor(void)
{
 if(RTab != NULL)
 {
  free(RTab);
  RTab = NULL;
 }
}

/* Oddziel tysiące spacjami, aby łatwiej było zobaczyć dotychczasową sumę */
char * acptr_Class::LadnieTysiacami(void)
{
    static char local_buff[SEP_BF_SIZE];
    /* Funkcja snprintf() zwraca liczbę cyfr wartości:
       - końcowe zero nie jest liczone,
       - jeżeli bufor jest za mały (np. 2 bajty dla liczby 1 milion), to zwracane jest 7
       Końcowe zero zawsze występuje w zapisywanym napisie.
       Bufor jest wystarczająco długi, jeśli po zapisie liczby
       wartość zwrotna jest <= SEP_BF_SIZE - 2
    */
#ifdef PLATFORM_LINUX
    int total_cnt = snprintf(local_buff, SEP_BF_SIZE, "%lld", s_so_far);
#endif
#ifdef PLATFORM_WIN
    int total_cnt = snprintf(local_buff, SEP_BF_SIZE, "%I64d", s_so_far);
#endif
    int src_pos, dst_pos, digits_taken;
    if(total_cnt >= SEP_BF_SIZE - 1)
    {
        printf("Rozmiar bufora to %d bajtow.\n", SEP_BF_SIZE);
        printf("Liczba zostala skonwertowana do %d cyfr.\n", total_cnt);
        printf("Jest potrzebny jeszcze dodatkowy bajt dla zera konczacego napis.\n");
        printf("A dodatkowo warto nie wykorzystac ostatniego bajtu, aby bylo widac, ze nie przepelniono buforu.\n");
        SygError("Bufor ma za mala pojemnosc.");
        exit(1);
    }
    src_pos = total_cnt; /* To liczba cyfr, bez liczenia bajtu zerowego kończącego napis */
    dst_pos = SEP_BF_SIZE - 1;
    digits_taken = 0;
    local_buff[dst_pos] = local_buff[src_pos]; /* Prześlij zerowy bajt ASCII NULL (kończy zapis cyfry) */
    while(1)
    {
        if(
            (src_pos > 0) &&
            (src_pos < dst_pos) &&
            (digits_taken > 0) &&
            ((digits_taken % 3) == 0)
            )
        {
            if(dst_pos <= 0)
            {
                break;
            }
            dst_pos --;
            local_buff[dst_pos] = ' '; /* Wpisz spację rozdzielającą tysiące */
        }
        if(src_pos <= 0)
        {
            break; /* Koniec danych do pobrania */
        }
        else
        {
            src_pos --;
        }
        if(dst_pos <= 0)
        {
            break; /* Koniec miejsca do zapisywania */
        }
        else
        {
            dst_pos --;
        }
        local_buff[dst_pos] = local_buff[src_pos]; /* Prześlij jedną cyfrę */
        digits_taken ++;
    }
    return local_buff + dst_pos;
}

/* Do zmiennej t5 wrzucamy aktualny czas */
void acptr_Class::AktualnyCzas(void)
{
#ifdef PLATFORM_LINUX
 if(gettimeofday(& t5, NULL) != 0)
 {
  SygErrorParm("Funkcja gettimeofday() zwróciła errno = %d\n", errno);
  ZmienStan(STATE_OFF, KIER_PROSTO);
 }
#endif

#ifdef PLATFORM_WIN
 t5 = GetTickCount();
#endif
}


#ifdef PLATFORM_LINUX
/* Wylicza różnicę czasu b - a. Jeśli różnica jest dodatnia, to zwracamy 1 (można
 * dzielić przez tę wartość), a w przeciwnym razie zwracamy 0. */
int acptr_Class::WyliczCzasDelta(struct timeval * a, struct timeval * b)
{
 int status;
 struct timeval tmp;
 struct timeval * dest;

 dest = & tmp;
 status = 0;
 if(b->tv_sec >= a->tv_sec)
 {
  dest->tv_sec = b->tv_sec - a->tv_sec;
  if(b->tv_usec >= a->tv_usec)
  {
   dest->tv_usec = b->tv_usec - a->tv_usec;
  }
  else
  {
   /* Trzeba wykonac pozyczke z pelnych sekund */
   if(dest->tv_sec > 0)
   {
    dest->tv_usec = 1000000 + b->tv_usec - a->tv_usec;
    dest->tv_sec --;
   }
   else
   {
    /* Mamy sytuację awaryjną - nie ma od kogo pożyczyć */
    dest->tv_sec = dest->tv_usec = 0;
   }
  }
  if(dest->tv_sec > 0 || dest->tv_usec > 0)
  {
   tdiff = (float) (dest->tv_sec + (1e-6 * dest->tv_usec));
   status = 1;
  }
 }
 return status;
}
#endif

#ifdef PLATFORM_WIN
/* To samo co wyżej, ale dla MS Windows */
int acptr_Class::WyliczCzasDelta(DWORD a, DWORD b)
{
 int status;

 status = 0;
 if(a < b)
 {
  tdiff = (float) ((b - a) * (1e-3));
  status = 1;
 }
 return status;
}
#endif

/* Sprawdza, czy t4 <= t5 na osi czasu. Jeśli tak, to wartość zwrotna = 1,
w przeciwnym razie wartość zwrotna to 0. */
int acptr_Class::Chronologicznie(void)
{
 int status;
 status = 0;
#ifdef PLATFORM_LINUX
 if(t4.tv_sec < t5.tv_sec ||
    t4.tv_sec == t5.tv_sec && t4.tv_usec <= t5.tv_usec)
 {
  status = 1;
 }
#endif

#ifdef PLATFORM_WIN
 if(t4 <= t5)
 {
  status = 1;
 }
#endif
 return status;
}

/* Generuje raport co krótki okres czasu, wielokrotnie w czasie transmisji */
void acptr_Class::RaportOkresowy(void)
{
#ifdef PLATFORM_LINUX
 if(WyliczCzasDelta( & t3, & t5))
 {
  printf("Czas %g s, %lld B, %s B, transfer %g KB/s\n", tdiff, s1, LadnieTysiacami(),
         s1 / tdiff / 1024);
 }
 else
 {
  printf("zerowy okres czasu\n");
 }
#endif

#ifdef PLATFORM_WIN
 if(WyliczCzasDelta(t3, t5))
 {
  printf("Czas %g s, %I64d B, %s B, transfer %g KB/s\n", tdiff, s1, LadnieTysiacami(),
         s1 / tdiff / 1024);
 }
 else
 {
  printf("zerowy okres czasu\n");
 }
#endif
 s1 = 0;
 t3 = t5;
}

/* Generuje końcowy raport */
void acptr_Class::RaportKoncowy(void)
{
#ifdef PLATFORM_LINUX
 if(WyliczCzasDelta( & t0, & t5))
 {
  printf("Czas total %g s, %lld B, transfer %g KB/s\n", tdiff, s0,
         s0 / tdiff / 1024);
 }
 else
 {
  printf("zerowy okres czasu\n");
 }
#endif

#ifdef PLATFORM_WIN
 if(WyliczCzasDelta(t0, t5))
 {
  printf("Czas total %g s, %I64d B, transfer %g KB/s\n", tdiff, s0,
         s0 / tdiff / 1024);
 }
 else
 {
  printf("zerowy okres czasu\n");
 }
#endif
}

/* Informuje o tym, że interesują nas przyrosty wartości w poszczególnych
 * okresach, a nie wartości od początku transmisji. W wyniku tego dostajemy o
 * jeden mniej rezultat na ekranie, niż jest zarejestrowanych wartości. */
#define DELTY 1

/* Generuje raport o oczekiwaniu na odczyt */
void acptr_Class::DumpRTable(void)
{
 int i;

 if(RMax > 0 && RAkt > 0)
 {
#if DELTY
  for(i = 0; i < RAkt - 1; i++)
#else
  for(i = 0; i < RAkt; i++)
#endif
  {
#ifdef PLATFORM_LINUX
#if DELTY
   if(WyliczCzasDelta(& RTab[i], & RTab[i + 1]))
#else
   if(WyliczCzasDelta( & t0, & RTab[i]))
#endif
#endif
#ifdef PLATFORM_WIN
#if DELTY
   if(WyliczCzasDelta(RTab[i], RTab[i + 1]))
#else
   if(WyliczCzasDelta(t0, RTab[i]))
#endif
#endif
   {
    printf("%g\n", tdiff);
   }
   else
   {
    printf("0\n");
   }
  }
  RMax = RAkt = 0;
 }
}

int acptr_Class::Init(int argc, char * argv[])
{
 int status;
 int nr; /* numer aktualnego parametru */
 int parmok; /* Parametry OK */

 status = RESULT_OFF;
 RMax = 0;
 RAkt = 0;
 SLmt = 0;
 SAkt = 0;
 parmok = 1;
 if(argc >= 2)
 {
  nr = 1;
  if( ! strcmp(argv[nr], "-r"))
  {
   /* Parametry: nazwa "acptr", (opcjonalnie "-r", liczba_pomiarów),
      liczba_sekund */
   if(argc >= 2 + 2)
   {
    nr ++;
    RMax = atoi(argv[nr]);
    if(RMax > 0)
    {
     nr++;
     RTab = (TCZAS *) malloc(RMax * sizeof(TCZAS));
     if(RTab != NULL)
     {
      /* Zerowanie po to, aby dotknąć pamięci i ją przydzielić */
      memset(RTab, 0, RMax * sizeof(TCZAS));
     }
     else
     {
      SygErrorParm("Nie dostałem obszaru pamięci dla tablicy %d elementów\n", RMax);
      parmok = 0;
     }
    }
    else
    {
     SygErrorParm("Nie udało się uzyskać dodatniej liczby z napisu %s\n", argv[nr]);
     parmok = 0;
    }
   }
   else
   {
    SygErrorParm("Mamy za mało parametrów dla opcji \"-r\" %d\n", argc);
    parmok = 0;
   }
  }
  else
  {
   if( ! strcmp(argv[1], "-s"))
   {
    /* Parametry: nazwa "acptr", (opcjonalnie "-s"), co_ile_odwołań */
    if(argc >= 2 + 1)
    {
     nr ++; /* Przesuwamy się nad co_ile_odwołań */
     /* Ta wartość jest symboliczna, jeśli jest różna od zero, to oznacza, że
      * "t2" jest liczbą wywołań, a nie sekund, po których generujemy kolejny
      * raport na ekranie. */
     SLmt = 1;
    }
    else
    {
     SygErrorParm("Mamy za mało parametrów dla opcji \"-s\" %d\n", argc);
     parmok = 0;
    }
   }
  }
  t2 = atoi(argv[nr]);
  nr ++;
  if(t2 >= 0)
  {
   s0 = 0;
   AktualnyCzas(); /* Rezultat w t5 */
   t0 = t5;
   if(t2)
   {
    /* Będziemy generować okresowe raporty */
    s1 = 0;
    s_so_far = 0;
    t3 = t5;
    /* Dla "SLmt <> 0" będziemy wyświetlać raporty co pewną liczbę odczytów
     * z poprzedzającego magazynu, dlatego nie jest ważna wartość "t4" */
    if( ! SLmt)
    {
     /* Tradycyjnie odmierzamy raporty co pewien czas */
     t4 = t0; /* Zaczynamy od momentu startu */
#ifdef PLATFORM_LINUX
     t4.tv_sec += t2;
#endif

#ifdef PLATFORM_WIN
     t4 += (t2 * 1000);
#endif
    }
   }
   status = nr;
  }
  else
  {
   SygErrorParm("Dziwna wartość interwału czasowego %d\n", t2);
  }
 }
 else
 {
  SygError("Nie podano parametru - liczby nieujemnej");
 }
 return status;
}

int acptr_Class::Work(int kmn, Byte * poczatek, int ile)
{
 int status;

 status = RESULT_OFF;
 switch(kmn)
 {
  case SAND_SR:
  {
   status = ile;
   s0 +=
#ifdef PLATFORM_LINUX
   (long long)
#endif
#ifdef PLATFORM_WIN
   (__int64)
#endif
   ile;
   if(t2 || RMax != 0)
   {
    AktualnyCzas(); /* Rezultat w t5 */
    if(RAkt < RMax)
    {
     RTab[RAkt++] = t5;
    }
    else
    {
     DumpRTable();
    }
   }
   if(t2)
   {
    s3 =
#ifdef PLATFORM_LINUX
    (long long)
#endif
#ifdef PLATFORM_WIN
    (__int64)
#endif
    ile;
    s1 += s3;
    s_so_far += s3;
    if(SLmt)
    {
     SAkt ++;
    }
    if(SLmt ? (SAkt == t2) : Chronologicznie()) /* Dotyczy t4, t5 */
    {
     RaportOkresowy();
     if(SLmt) /* Po raporcie musimy się przygotować do następnego raportu */
     {
      SAkt = 0; /* Zaczynamy liczyć od nowa */
     }
     else
     {
      /* Wyznacz moment następnego wyświetlenia raportu */
      while(Chronologicznie())
      {
#ifdef PLATFORM_LINUX
       t4.tv_sec += t2;
#endif

#ifdef PLATFORM_WIN
       t4 += (t2 * 1000);
#endif
      }
     }
    }
   }
   break;
  }
  case SAND_EOF:
  {
   status = RESULT_EOF;
   AktualnyCzas(); /* Rezultat w t5 */
   if(t2)
   {
    RaportOkresowy();
   }
   DumpRTable(); /* Być może tablica nie została zrzucona */
   RaportKoncowy();
   break;
  }
  case SAND_OFF:
  {
   ZmienStan(STATE_OFF, KIER_PROSTO);
   status = RESULT_OFF;
   break;
  }
  default:
  {
   SygErrorParm("Nieznany komunikat: %d", kmn);
   ZmienStan(STATE_OFF, KIER_PROSTO);
  }
 }
 return status;
}
