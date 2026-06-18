#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define ROZMIAR 80

void* konwertuj(void* arg)
{
 char* linia = (char*)arg;
 int i;
 
 for(i = 0; linia[i] != '\0'; i++)
 {
  if(linia[i] >= '0' && linia[i] <= '9')
  {
   linia[i] = '9' - (linia[i] - '0');
  }
 }
 return NULL;
}

int main()
{
 FILE *plik_we, *plik_wy;
 pthread_t watki[5];
 char linie[5][ROZMIAR];
 int liczba_linii = 0;
 int i;

 plik_we = fopen("wejscie.txt", "r");
 if (plik_we == NULL) return 1;

 while (liczba_linii < 5 && fgets(linie[liczba_linii], ROZMIAR, plik_we) != NULL)
 {
  pthread_create(&watki[liczba_linii], NULL, konwertuj, (void*)linie[liczba_linii]);
  liczba_linii++;
 }
 fclose(plik_we);

 for (i = 0; i < liczba_linii; i++)
 {
  pthread_join(watki[i], NULL);
 }

 plik_wy = fopen("wyjscie.txt", "w");
 if (plik_wy == NULL) return 1;

 for (i = 0; i < liczba_linii; i++)
 {
  fputs(linie[i], plik_wy);
 }
 fclose(plik_wy);

 return 0;
}
