#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <asm/byteorder.h>
#include <string.h>
#include <getopt.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

void recibirParametros(int argc, char *argv[], int* numeroImagenes, char* nombreMascara, int* umbral, int* bandera) {
  char* nomb = malloc(sizeof(char)*20);
  //char* nombreMascara = malloc(sizeof(char)*20);
  //int* umbral = malloc(sizeof(int));
  
  int c;
  *bandera = 0;

  while ((c = getopt(argc, argv, "c:m:n:b")) != -1) {
    switch (c) {
      case 'c':
        sscanf(optarg, "%d", numeroImagenes);
        if (numeroImagenes < 0)
        {
            printf("la cantidad de imagenes a leer no puede ser negativa\n");
            exit(1);
        }
        break;      
      case 'm':
        sscanf(optarg, "%s", nombreMascara);
        break;
      case 'n':
        sscanf(optarg, "%d", umbral);
        break;
      case 'b':
        *bandera = 1;
        break;
      case '?':
        if (optopt == 'c') {
          fprintf(stderr, "opcion -%c requiere un argumento.\n", optopt);
        }
        else if (isprint(optopt)) {
          fprintf(stderr, "opcion desconocida '-%c'.\n", optopt);
        }
        else
          fprintf(stderr, "opcion con caracter desconocido '\\x%x'.\n", optopt);

      default:
        abort();
    }
  }
  if (*bandera == 1) {
    //printf("%s\n", nomb);
  }

}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char *argv[]) {
    char nombreMascara[20];
    int umbral, bandera, numeroImagenes;
    int pipes[2];
    pipe(pipes);
    recibirParametros(argc, argv, &numeroImagenes, nombreMascara, &umbral, &bandera);
    char fdRead[100];
    char sNumeroImagenes[3];
    char sUmbral[4];
    char sBandera[2];
    sprintf(fdRead, "%d", pipes[READ]);
    sprintf(sNumeroImagenes, "%d", numeroImagenes);
    sprintf(sUmbral, "%d", umbral);
    sprintf(sBandera, "%d", bandera);
    char* argumentos[7] = {"./read", fdRead, sNumeroImagenes, nombreMascara, sUmbral, sBandera, NULL};
    int pid;
    pid = fork();
    if (pid == 0) //Hijo
    {
      close(pipes[WRITE]);
      execv("./read", argumentos);
    }
    else
    {
      close(pipes[READ]);
      write(pipes[WRITE], "alo", 10);
      int status;
      wait(&status);
    }
    
    return 0;
}