#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define ROZMIAR 1024

int main()
{
 int potok_we[2];
 int potok_wy[2];
 pid_t pid;

 pipe(potok_we);
 pipe(potok_wy);

 pid = fork();

 if (pid == 0)
 {
  char bufor_odczyt[ROZMIAR];
  char bufor_zapis[ROZMIAR * 2];
  int bajty, i, j;

  close(potok_we[1]);
  close(potok_wy[0]);

  while ((bajty = read(potok_we[0], bufor_odczyt, ROZMIAR)) > 0)
  {
   j = 0;
   for (i = 0; i < bajty; i++)
   {
    bufor_zapis[j++] = bufor_odczyt[i];
    if (bufor_odczyt[i] >= 'A' && bufor_odczyt[i] <= 'Z')
    {
     bufor_zapis[j++] = ' ';
    }
   }
   write(potok_wy[1], bufor_zapis, j);
  }

  close(potok_we[0]);
  close(potok_wy[1]);
 }
 else
 {
  char bufor[ROZMIAR];
  int bajty;
  FILE *plik_we, *plik_wy;

  close(potok_we[0]);
  close(potok_wy[1]);

  plik_we = fopen("wejscie.txt", "r");
  if (plik_we != NULL)
  {
   while ((bajty = fread(bufor, 1, ROZMIAR, plik_we)) > 0)
   {
    write(potok_we[1], bufor, bajty);
   }
   fclose(plik_we);
  }
  close(potok_we[1]); 

  plik_wy = fopen("wyjscie.txt", "w");
  if (plik_wy != NULL)
  {
   while ((bajty = read(potok_wy[0], bufor, ROZMIAR)) > 0)
   {
    fwrite(bufor, 1, bajty, plik_wy);
   }
   fclose(plik_wy);
  }
  close(potok_wy[0]);

  wait(NULL);
 }

 return 0;
}
