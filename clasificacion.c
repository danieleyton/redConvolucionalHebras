#include <stdlib.h>
#include <stdio.h>
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

//entradas: puntero a estructura matriz que contiene la imagen luego del proceso pooling
//          entero que representa el umbral para clasificar el porcentaje de pixeles negros de la imagen
//funcionamiento: clasifica si la imagen es nearly black, de acuerdo a si su porcentaje de pixeles negros es mayor o menor al umbral
//salidas: No tiene salida, se muestra por pantalla si la imagen es nearly black o no
void clasificacion(matriz* imagen, int umbral, int bandera)
{
    int pixeles_negros = 0;
    for (int i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            if (imagen->matriz[i][j] == 0)
            {
                pixeles_negros++;
            }
            
        }
        
    }

    int total_pixeles = imagen->filas * imagen->columnas;
    int porcentaje_negro = (pixeles_negros * 100)/total_pixeles;
    printf("porcentaje negro: %d\n", porcentaje_negro);

    if (porcentaje_negro >= umbral && bandera)
    {
        printf("la imagen es nearly black\n");
    }
    else if(bandera)
    {
        printf("la imagen no es nearly black\n");
    }

}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char const *argv[])
{
    char mensaje[10];
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    int umbral = atoi(argv[4]);
    int bandera = atoi(argv[5]);
    matriz* imagen = crearMatriz(filas, columnas);
    //int leido = read(atoi(argv[1]), mensaje, 10);
    int pipes[2];
    pipe(pipes);
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
        //printf("soy el proceso clasificacion y lei: %s\n", mensaje);
        clasificacion(imagen, umbral, bandera);
    }
    else
    {
        printf("parece que fallo\n");
    }
    
    int pid = fork();
    if (pid == 0)
    {
        char fdRead[5];
        char sHeight[5];
        char sWidth[5];
        sprintf(fdRead, "%d", pipes[0]);
        sprintf(sHeight, "%d", imagen->filas);
        sprintf(sWidth, "%d", imagen->columnas);
        char* argumentos[5] = {"./write", fdRead, sHeight, sWidth, NULL};
        close(pipes[1]);
        execv("./write", argumentos);
        perror("el execv fallo\n");
    }
    else
    {
        close(pipes[0]);
        for (int i = 0; i < imagen->filas; i++)
        {
            for (int j = 0; j < imagen->columnas; j++)
            {
                write(pipes[1], &imagen->matriz[i][j], sizeof(int));
            }
            
        }

        int status;
        wait(&status);
        
    }
    
    return 0;
}
