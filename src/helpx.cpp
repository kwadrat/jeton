#include "scmpx.h"
#include <stdio.h>

#define THIS_PROGRAM_VERSION "1.00"

/* Funkcja zwraca 1, gdy użytkownik prosi o pomoc w obsłudze programu. Jeśli nie zostanie
rozpoznana żadna opcja odnośnie pomocy, to zwracana jest wartość 0. */
int WantHelp(char *s)
{
 int status = 0;
 if( ! CaseStrcmp(s, "-h") ||
     ! CaseStrcmp(s, "--help") ||
     ! CaseStrcmp(s, "/h") ||
     ! CaseStrcmp(s, "/?") ||
     ! CaseStrcmp(s, "-?") ||
     ! CaseStrcmp(s, "help") ||
     ! CaseStrcmp(s, ""))
 {
  status = 1;
 }
 return status;
}

void ShowHelp(int, char *[])
{
 puts("Program jeton v" THIS_PROGRAM_VERSION " - kopiowanie bloków bajtów");
 puts("qaz - do uzupełnienia");
 puts("si [-<");
 puts("fi [-<");
 puts("tis [-<");
 puts("tic [-<");
 puts("smar <->");
 puts("kopier <->");
 puts("md5 <->");
 puts("cat >-<");
 puts("y >->>");
 puts("so >-]");
 puts("fo >-]");
 puts("tos >-]");
 puts("toc >-]");
}
