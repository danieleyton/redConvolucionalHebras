#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "estructuras.h"

matriz* crearMatriz(int filas, int columnas)
{
    matriz* nueva = malloc(sizeof(matriz));
    nueva->filas = filas;
    nueva->columnas = columnas;

    nueva->matriz = malloc(sizeof(int*)*filas);

    for (int i = 0; i < filas; i++)
    {
        nueva->matriz[i] = malloc(sizeof(int)*columnas);
    }

    return nueva;
    
}

//entradas: puntero a estructura matriz que representa la imagen luego de haberle aplicado la convolucion
//funcionamiento: se aplica un proceso de rectificacion a la imagen, donde si hay pixeles con valor negativo
// estos son cambiados a valor 0
//salidas: puntero a estructura matriz que representa la imagen rectificcada, sin pixeles con valor negativo
void rectificacion(matriz* imagen)
{
    for (int i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            if (imagen->matriz[i][j] < 0)
            {
                imagen->matriz[i][j] = 0;
            }
        }
    }   
}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char const *argv[])
{
    //matriz a;
    //rectificacion(a);    
    char mensaje[10];
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    matriz* imagen = crearMatriz(filas, columnas);
    int pipes[2];
    pipe(pipes);
    //int leido = read(atoi(argv[1]), mensaje, 10);    
    int leido;
    for (int i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            leido = read(atoi(argv[1]), &imagen->matriz[i][j], sizeof(int));
        }
        
    }
    
    if (leido > 0)
    {
        //printf("soy el proceso rectificacion y lei: %s\n", mensaje);
        rectificacion(imagen);
        //printf("se hizo la rectificacion\n");
    }
    else
    {
        printf("leido: %d\n", leido);
    }
    
    int pid = fork();
    if (pid == 0)
    {
        char fdRead[10];
        char sHeight[5];
        char sWidth[5];
        char sUmbral[4];
        char sBandera[2];
        sprintf(fdRead, "%d", pipes[0]);
        sprintf(sHeight, "%d", imagen->filas);
        sprintf(sWidth, "%d", imagen->columnas);
        sprintf(sUmbral, "%d", atoi(argv[4]));
        sprintf(sBandera, "%d", atoi(argv[5]));
        char* argumentos[7] = {"./pooling", fdRead, sHeight, sWidth, sUmbral, sBandera, NULL};
        close(pipes[1]);
        execv("./pooling", argumentos);
        printf("el execv fallo\n");
    }
    else
    {
        close(pipes[0]);
        //write(pipes[1], "alola", 10);
        for (int i = 0; i < imagen->filas; i++)
        {
            for (int j = 0; j < imagen->columnas; j++)
            {
                write(pipes[1], &imagen->matriz[i][j], sizeof(int));
            }
            
        }
        
    }
    int status;
    wait(&status);
    
    return 0;
}
