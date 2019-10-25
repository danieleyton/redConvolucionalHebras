#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"

//entradas:
//funcionamiento:
//salidas:
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

void liberarMatriz(matriz* imagen)
{
    for (int i = 0; i < imagen->filas; i++)
    {
        free(imagen->matriz[i]);
    }
    free(imagen->matriz);

    free(imagen);
    
}