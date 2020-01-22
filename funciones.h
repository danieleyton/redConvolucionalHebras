#include "estructuras.h"
#include <png.h>
#include <stddef.h>

#ifndef FUNCIONES_H
#define FUNCIONES_H

void* leer_archivo_png(void *nombreArchivo);
matriz *crearMatriz(int filas, int columnas);
void liberarMatriz(matriz* imagen);
matriz* leerFiltro(char* nombreFiltro);
matriz* convolucion(matriz* imagen, matriz* filtro);
void write_png_file(char* filename, matriz* imagen);
void rectificacion(matriz* imagen);
matriz* crearMatrizPooling(int filas, int columnas);
matriz* pooling(matriz* imagen);
matriz* png_a_matriz(png_bytep* png, int filas, int columnas);
void casteo_a_pngByte(matriz* imagen, png_bytep* pngMatriz);
void clasificacion(matriz* imagen, int umbral, int bandera, int numeroHebras);

#endif // !FUNCIONES_H