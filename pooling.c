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

//entradas: entero que represemta el numero de filas de la imagen original
//          entero que representa el numero de columnas de la imagen original
//funcionamiento: se crea una matriz con dimensiones resultantes a haber aplicado un proceso de pooling
// con un filtro de 2x2 a la imagen original, por lo que tendra la mitad de las dimensiones originales
//salidas: (memoria de) matriz para almacenar el resultado del pooling
matriz* crearMatrizPooling(int filas, int columnas)
{
    matriz* matrizPooling = malloc(sizeof(matriz));

    if (filas % 2 == 0)
    {
        matrizPooling->filas = filas/2;
    }
    else
    {
        matrizPooling->filas = (filas+1)/2;
    }

    if (columnas % 2 == 0)
    {
        matrizPooling->columnas = columnas/2;
    }
    else
    {
        matrizPooling->columnas = (columnas+1)/2;
    }
    
    matrizPooling->matriz = malloc(sizeof(int*)*matrizPooling->filas);

    for (int i = 0; i < matrizPooling->filas; i++)
    {
        matrizPooling->matriz[i] = malloc(sizeof(int)*matrizPooling->columnas);
    }
    
    return matrizPooling;
    
}

//entradas: puntero a estructura matriz que contiene la imagen luego de haber pasado por el proceso de rectificacion
//funcionamiento: aplica proceso de pooling con filtro de 2x2 a la imagen, esto escala la imagen a la mitad
//salidas: puntero a matriz que contiene la imagen a la mitad de su escala original
matriz* pooling(matriz* imagen)
{
    int upLeft;
    int upRight;
    int downLeft;
    int downRight;
    int k = 0;
    int l = 0;

    matriz* resultado = crearMatrizPooling(imagen->filas, imagen->columnas);

    for (int i = 0; i < imagen->filas; i+=2)
    {
        l = 0;
        for (int j = 0; j < imagen->columnas; j+=2)
        {
            upLeft = imagen->matriz[i][j];
            if (i+1 >= imagen->filas)
            {
                if (j+1 >= imagen->columnas)
                {
                    downRight = 0;
                }

                downLeft = 0;
                
            }
            else if (j+1 >= imagen->columnas)
            {
                upRight = 0;
            }
            else
            {
                upRight = imagen->matriz[i][j+1];
                downLeft = imagen->matriz[i+1][j];
                downRight = imagen->matriz[i+1][j+1];
            }

            int max = (upLeft > upRight ? upLeft : upRight);

            max = (downLeft > downRight ? (downLeft > max ? downLeft : max) : (downRight > max ? downRight : max));

            resultado->matriz[k][l] = max;

            l = l + 1;
            
        }

        k++;
        
    }

    return resultado;
    
}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char const *argv[])
{
    //matriz a;
    //pooling(a);
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
    
    matriz* resultado;
    if (leido > 0)
    {
        //printf("soy el proceso pooling y lei: %s\n", mensaje);
        resultado = pooling(imagen);
        /*for (int i = 0; i < resultado->filas; i++)
        {
            for (int j = 0; j < resultado->columnas; j++)
            {
                printf("%3d", resultado->matriz[i][j]);
            }

            printf("\n");
            
        }*/
    }
    else
    {
        printf("parece que fallo, leido: %d\n", leido);
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
        sprintf(sHeight, "%d", resultado->filas);
        sprintf(sWidth, "%d", resultado->columnas);
        sprintf(sUmbral, "%d", atoi(argv[4]));
        sprintf(sBandera, "%d", atoi(argv[5]));
        char* argumentos[7] = {"./clasificacion", fdRead, sHeight, sWidth, sUmbral, sBandera, NULL};
        close(pipes[1]);
        execv("./clasificacion", argumentos);
        printf("el execv fallo\n");
    }
    else
    {
        close(pipes[0]);
        //write(pipes[1], "olaalo", 10);
        for (int i = 0; i < resultado->filas; i++)
        {
            for (int j = 0; j < resultado->columnas; j++)
            {
                write(pipes[1], &resultado->matriz[i][j], sizeof(int));
            }
            
        }

        int status;
        wait(&status);
        
    }
    
    return 0;
}
