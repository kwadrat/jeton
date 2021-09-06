/* Część główna programu - interpretacja linii komend */

#include "typy.h"
#include "helpx.h"
#include "logx.h"
#include "masterx.h"
#include "rvrx.h"
#include "jeton.h"

#include <assert.h>

MasterClass TopMember; /* Tu jest definicja jedynego obiektu tej klasy */

#define TESTOWANIE 0

#if TESTOWANIE
#define ROZMIAR_TAB 6
char * Tablica[ROZMIAR_TAB] = {
"gi",
"1",
"kopier",
"1",
"acptr",
"1"
};
#endif

int error_occured;

int main(int argc, char * argv[])
{
 /* Potrzebujemy tego rozmiaru w procesie "smar" */
 assert(sizeof(int) == ROZM_INT);

#if ! TESTOWANIE

 if(argc > 1)
 {
  if(WantHelp(argv[1]))
  {
   ShowHelp(argc, argv);
  }
  else /* Nie chcemy pomocy */
  {
   if(CreateChainLogic(argc - 1, argv + 1))
   {
    /* Circle in the sand (Belinda Carlile?) - uruchom łańcuch procesów */
    TopMember.RunProcesses();
   }
   else
   {
    SygError("Kończę z powodu błędu przetwarzania.");
   }
  }
 }
 else
 {
  SygError("Brak parametrów - nie wiem, co mam robić!");
 }

#else
 if(CreateChainLogic(ROZMIAR_TAB, Tablica))
 {
  /* Circle in the sand (Belinda Carlisle) - uruchom łańcuch procesów */
  TopMember.RunProcesses();
 }
 else
 {
  SygError("Kończę z powodu błędu przetwarzania.");
 }

#endif
 TopMember.LocalDestructor(); /* Musimy pozamykać pliki */
#if AKTYWNY_RIVER
 TopRiver.Finish();
#endif
 return error_occured;
}
