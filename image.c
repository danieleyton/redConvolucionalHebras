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

char* recibirParametros(int argc, char *argv[], char* nombreMascara, int umbral, int* bandera) {
  char* nomb = malloc(sizeof(char)*20);
  //char* nombreMascara = malloc(sizeof(char)*20);
  //int* umbral = malloc(sizeof(int));
  
  int c;
  //int bandera = 0;

  while ((c = getopt(argc, argv, "c:m:n:b")) != -1) {
    switch (c) {
      case 'c':
        //sscanf(optarg, "%s", nomb);
        break;      
      case 'm':
        //bandera = 1;
        sscanf(optarg, "%s", nombreMascara);
        break;
      case 'n':
        sscanf(optarg, "%d", &umbral);
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

  return nomb;

}

int main(int argc, char *argv[]) {
    char* nombreMascara = malloc(sizeof(char)*20);
    int umbral, bandera;
    int pipes[2];
    pipe(pipes);
    //printPNG(argc, argv, nombreMascara, umbral, bandera);
    char buffer[100];
    sprintf(buffer, "%d", pipes[READ]);
    char* argumentos[3] = {"./read", buffer, NULL};
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