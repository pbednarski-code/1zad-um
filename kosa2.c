#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define ROZMIAR 1024

int czytaj(int deskryptor, void* bufor, int rozmiar)
{
 int odczytane=0;
 int wynik;

 while (odczytane<rozmiar)
 {
  wynik=read(deskryptor, (char*)bufor+odczytane, rozmiar-odczytane);
  if (wynik<=0)
   return wynik;
  odczytane+=wynik;
 }

 return odczytane;
}

int pisz(int deskryptor, void* bufor, int rozmiar)
{
 int zapisane=0;
 int wynik;

 while (zapisane<rozmiar)
 {
  wynik=write(deskryptor, (char*)bufor+zapisane, rozmiar-zapisane);
  if (wynik<=0)
   return wynik;
  zapisane+=wynik;
 }

 return zapisane;
}

int main(int argc, char* argv[])
{
 int doPotomka[2];
 int doRodzica[2];
 pid_t pid;
 FILE *wejscie, *wyjscie;
 char linia[ROZMIAR+1];
 char bufor[ROZMIAR+1];
 char wynik[2*ROZMIAR+1];
 char* plikWejsciowy;
 char* plikWyjsciowy;
 int dlugosc;
 int nowaDlugosc;
 int i, j;

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

 pipe(doPotomka);
 pipe(doRodzica);

 pid=fork();

 if (pid==0)
 {
  close(doPotomka[1]);
  close(doRodzica[0]);

  while (czytaj(doPotomka[0], &dlugosc, sizeof(int))>0)
  {
   if (dlugosc==0)
    break;

   czytaj(doPotomka[0], bufor, dlugosc);
   bufor[dlugosc]='\0';

   j=0;
   for (i=0; bufor[i]!='\0'; i++)
   {
    wynik[j++]=bufor[i];

    if (bufor[i]>='A' && bufor[i]<='Z')
     wynik[j++]=' ';
   }

   wynik[j]='\0';
   nowaDlugosc=strlen(wynik);

   pisz(doRodzica[1], &nowaDlugosc, sizeof(int));
   pisz(doRodzica[1], wynik, nowaDlugosc);
  }

  close(doPotomka[0]);
  close(doRodzica[1]);

  return 0;
 }
 else
 {
  close(doPotomka[0]);
  close(doRodzica[1]);

  wejscie=fopen(plikWejsciowy, "r");
  if (wejscie==NULL)
  {
   printf("Nie mozna otworzyc pliku wejsciowego\n");
   return 1;
  }

  wyjscie=fopen(plikWyjsciowy, "w");
  if (wyjscie==NULL)
  {
   printf("Nie mozna otworzyc pliku wyjsciowego\n");
   return 1;
  }

  while (fgets(linia, ROZMIAR, wejscie)!=NULL)
  {
   dlugosc=strlen(linia);

   pisz(doPotomka[1], &dlugosc, sizeof(int));
   pisz(doPotomka[1], linia, dlugosc);

   czytaj(doRodzica[0], &nowaDlugosc, sizeof(int));
   czytaj(doRodzica[0], wynik, nowaDlugosc);

   fwrite(wynik, sizeof(char), nowaDlugosc, wyjscie);
  }

  dlugosc=0;
  pisz(doPotomka[1], &dlugosc, sizeof(int));

  fclose(wejscie);
  fclose(wyjscie);

  close(doPotomka[1]);
  close(doRodzica[0]);

  wait(NULL);
 }

 return 0;
}
