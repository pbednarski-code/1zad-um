#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define ROZMIAR 80
#define ILE 5

struct dane
{
 char linia[ROZMIAR+1];
};

void* zamien(void* arg)
{
 struct dane* d=(struct dane*)arg;
 int i;

 for (i=0; d->linia[i]!='\0'; i++)
 {
  if (d->linia[i]>='0' && d->linia[i]<='9')
   d->linia[i]='9'-(d->linia[i]-'0');
 }

 return NULL;
}

int main(int argc, char* argv[])
{
 FILE *wejscie, *wyjscie;
 pthread_t watki[ILE];
 struct dane linie[ILE];
 char* plikWejsciowy;
 char* plikWyjsciowy;
 int liczbaLinii=0;
 int i;

 if (argc>=3)
 {
  plikWejsciowy=argv[1];
  plikWyjsciowy=argv[2];
 }
 else
 {
  plikWejsciowy="wejscie.txt";
  plikWyjsciowy="wynik.txt";
 }

 wejscie=fopen(plikWejsciowy, "r");
 if (wejscie==NULL)
 {
  printf("Nie mozna otworzyc pliku wejsciowego\n");
  return 1;
 }

 while (liczbaLinii<ILE && fgets(linie[liczbaLinii].linia, ROZMIAR, wejscie)!=NULL)
 {
  pthread_create(&watki[liczbaLinii], NULL, zamien, (void*)&linie[liczbaLinii]);
  liczbaLinii++;
 }

 fclose(wejscie);

 for (i=0; i<liczbaLinii; i++)
  pthread_join(watki[i], NULL);

 wyjscie=fopen(plikWyjsciowy, "w");
 if (wyjscie==NULL)
 {
  printf("Nie mozna otworzyc pliku wyjsciowego\n");
  return 1;
 }

 for (i=0; i<liczbaLinii; i++)
  fprintf(wyjscie, "%s", linie[i].linia);

 fclose(wyjscie);

 return 0;
}
