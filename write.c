#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <png.h>
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

//entradas: puntero a estructura matriz que contiene la imagen luego de haber sido clasificada
//          puntero a png_bytep que representa el tipo de dato necesario para escribir la imagen mediante las funciones de png.h
//funcionamiento: se castean los datos de imagen para que pasen de int** a png_bytep*
//salidas: No hay salida, la informacion se escribe en el argumento pngMatriz de la funcion
void casteo_a_pngByte(matriz* imagen, png_bytep* pngMatriz)
{
    //printf("filas: %d\n", imagen->filas);
    for (int i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            //printf("i: %d, j: %d\n", i, j);
            pngMatriz[i][j] = (png_byte)imagen->matriz[i][j];            
        }
        
    }
    
}

//entradas: String(puntero a char) que representa el nombre de la imagen que se va a escribir
//funcionamiento: escribe un archivo de tipo .png a partir de una matriz de png_byte y tipos de datos png, info, definidos en la biblioteca png.h
//salidas: No tiene salida, un archivo es escrito
void write_png_file(char *filename, matriz* imagen) {
  //int y;

  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    imagen->columnas, imagen->filas,
    8,
    //PNG_COLOR_TYPE_RGBA,
    PNG_COLOR_TYPE_GRAY,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  png_bytep* row_pointers = malloc(sizeof(png_bytep)*imagen->filas);
  for (int i = 0; i < imagen->filas; i++)
  {
      row_pointers[i] = malloc(png_get_rowbytes(png,info));
  }
  
  casteo_a_pngByte(imagen, row_pointers);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  if (!row_pointers) abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < imagen->filas; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  png_destroy_write_struct(&png, &info);
}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char const *argv[])
{
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    matriz* imagen = crearMatriz(filas, columnas);
    int leido;
    for (int  i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            leido = read(atoi(argv[1]), &imagen->matriz[i][j], sizeof(int));
        }
        
    }

    if (leido > 0)
    {
        write_png_file("salida.png", imagen);
        printf("fin\n");
    }
    else
    {
        printf("parece que fallo, leido: %d", leido);
    }
    
    
    
    return 0;
}
