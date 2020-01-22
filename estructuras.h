#include <png.h>
#ifndef _STRUCTS_H_
#define _STRUCTS_H_

typedef struct matriz
{
    int** matriz;
    int filas;
    int columnas;
}matriz;

typedef struct buffer
{
    png_bytep* filas;
    int size;
    int ultimo;
    int pos;
}buffer;

typedef struct argumentos
{
    int numFilas;
    matriz* filtro;
    int bandera;
    matriz* resultado;
    int umbral;
    int numeroHebras;
}argumentos;


#endif // !_STRUCTS_H_
