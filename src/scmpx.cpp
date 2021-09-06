#include <ctype.h>

/* Funkcja sprawdza, czy dwa napisy są identyczne, nie biorąc pod uwagę
wielkości liter (Wartość zwrotna: 1 - różne, 0 - jednakowe napisy) */
int CaseStrcmp(char *a, const char *b)
{
 while(*a && *b)
 {
  if(toupper(*a) == toupper(*b))
  {
   a++;
   b++;
  }
  else
  {
   break; /* Znaleźliśmy różne znaki */
  }
 }
 return (*a == *b) ? 0 : 1;
}
