#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>
#include "estructuras.h"

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;

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

//entradas: String(puntero a char) que representa el nombre de la imagen que se va a leer
//funcionamiento: Lee una imagen de tipo .png y la almacena en una matriz de png_byte en formato de escala de grises(unsigned char)
//salidas: No tiene salida, la matriz es almacenada en una variable global
void leer_archivo_png(char *nombreArchivo) {
  FILE *archivo = fopen(nombreArchivo, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, archivo);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  /*if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);*/

  // These color_type don't have an alpha channel then fill it with 0xff.
  /*if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);*/

  //if(color_type == PNG_COLOR_TYPE_GRAY ||
     //color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    //png_set_gray_to_rgb(png);

    if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGBA)
    {
      png_set_rgb_to_gray_fixed(png, 1, 21268, 71514);
      png_set_expand_gray_1_2_4_to_8(png);
    }
    

  png_read_update_info(png, info);

  if (row_pointers) abort();

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);

  fclose(archivo);

  png_destroy_read_struct(&png, &info, NULL);
}

//entradas: No tiene entradas por argumento, emplea una variable global
//funcionamiento: convierte la matriz de tipo png_byte a matriz de int y la almacena en una estructura llamada matriz
//salidas: puntero a estructura matriz
matriz* png_a_matriz() {
  matriz* imagen = crearMatriz(height, width);

  for(int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for(int x = 0; x < width; x++) {
      //png_bytep px = &(row[x * 4]);
      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
      //printf("%3d", row[x]);
      imagen->matriz[y][x] = (int)row[x];
    }
    //printf("\n");
  }
  return imagen;
}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char *argv[]) {
  char mensaje[10];
  int numeroImagenes = atoi(argv[2]);
  int pipes[2];
  pipe(pipes);
  if (numeroImagenes > 0)
  {
    leer_archivo_png("imagen_1.png");
  }
  if (numeroImagenes <= 0)
  {
    exit(1);
  }
  
  matriz* imagen = png_a_matriz();
  
  int leido = read(atoi(argv[1]), mensaje, 10);
  if (leido > 0)
  {
    //printf("soy el proceso read y lei: %s\n", mensaje);
  }
  int pid = fork();  
  if (pid == 0)
  {
    char buffer[10];
    char sHeight[5];
    char sWidth[5];
    sprintf(buffer, "%d", pipes[0]);
    sprintf(sHeight, "%d", height);
    sprintf(sWidth, "%d", width);
    char* argumentos[8] = {"./convolucion", buffer, sHeight, sWidth, argv[3], argv[4], argv[5], NULL};  
    close(pipes[1]);
    execv("./convolucion", argumentos);
  }
  else
  {
    close(pipes[0]);
    for (int i = 0; i < height; i++)
    {
      for (int j = 0; j < width; j++)
      {
        write(pipes[1], &imagen->matriz[i][j], sizeof(int));
      }
      
    }
    
    int status;
    wait(&status);
  }  

  return 0;
}