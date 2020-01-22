#include <stdio.h>
#include <stdlib.h>
#include "estructuras.h"
#include "shared.h"

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

//entradas: String(puntero a char) que representa el nombre de la imagen que se va a leer
//funcionamiento: Lee una imagen de tipo .png y la almacena en una matriz de png_byte en formato de escala de grises(unsigned char)
//salidas: No tiene salida, la matriz es almacenada en una variable global
void* leer_archivo_png(void *nombreArchivo) {
  FILE *archivo = fopen((char*)nombreArchivo, "rb");

  //row_pointers = NULL;

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, archivo);

  printf("nombre: %s\n", nombreArchivo);

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

  for(int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for(int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      // Do something awesome for each pixel here...
      //printf("algoooooo\n");
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
      //printf("%3d", row_pointers[y][x]);
    }
    //printf("\n");
  }

  fclose(archivo);

  png_destroy_read_struct(&png, &info, NULL);
}

matriz* leerFiltro(char* nombreFiltro)
{
    FILE* archivo = fopen(nombreFiltro, "r");

    if (!archivo)
    {
        perror("error abriendo el archivo del filtro\n");
        exit(1);
    }
    

    matriz* filtro = crearMatriz(3, 3);

    for (int i = 0; i < 3; i++)
    {
        int primero;
        int segundo;
        int tercero;
        fscanf(archivo, "%d %d %d", &primero, &segundo, &tercero);
        filtro->matriz[i][0] = primero;
        filtro->matriz[i][1] = segundo;
        filtro->matriz[i][2] = tercero;
    }

    fclose(archivo);    

    return filtro;
}

//entradas: puntero a estructura matriz que representa la imagen a la que se la aplicara la convolucion
//          puntero a estructura matriz que representa el filtro que se le aplicara a la imagen en la convolucion
//funcionamiento: se le aplica un proceso de convolucion a la imagen mediante un filtro de suavizado
//salidas: puntero a estructura matriz con el resultado de el proceso de convolucion aplicado a imagen original
matriz* convolucion(matriz* imagen, matriz* filtro)
{
    matriz* resultado = crearMatriz(imagen->filas, imagen->columnas);

    for (int i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            for (int k = 0; k < filtro->filas; k++)
            {
                for (int l = 0; l < filtro->columnas; l++)
                {
                    if (i + (k - 1) >= 0 && j + (l - 1) >= 0 && i + (k-1) < imagen->filas && j + (l-1) < imagen->columnas) //agregar ii < imagen.filas && jj < imagen.columnas (mayor o igual?)
                    {
                        resultado->matriz[i][j] += imagen->matriz[i+(k-1)][j+(l-1)] * filtro->matriz[k][l];
                    }
                    else
                    {
                        resultado->matriz[i][j] += 0 * filtro->matriz[k][l];
                    }
                }
            }
            int aux = resultado->matriz[i][j]/(filtro->filas*filtro->columnas);
            if (aux <= 255)
            {
                resultado->matriz[i][j] = resultado->matriz[i][j]/(filtro->filas*filtro->columnas);
            }
            else
            {
                resultado->matriz[i][j] = 255;
            }
            
        }
    }
    return resultado;
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
void write_png_file(char *filename, matriz* imagen) { //matriz* imagen png_bytep* imagen
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
    //width, height,
    8,
    //PNG_COLOR_TYPE_RGBA,
    PNG_COLOR_TYPE_GRAY,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  /*png_bytep* row_pointers = malloc(sizeof(png_bytep)*height);
  for (int i = 0; i < height; i++)
  {
      row_pointers[i] = malloc(png_get_rowbytes(png,info));
  }*/
  
  casteo_a_pngByte(imagen, row_pointers);

  
  
  /*for(int y = 0; y < height; y++) {
    png_bytep row = row_pointers[y];
    for(int x = 0; x < width; x++) {
      png_bytep px = &(row[x * 4]);
      // Do something awesome for each pixel here...
      //printf("algoooooo\n");
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
      printf("%3d", row_pointers[y][x]);
    }
    printf("\n");
  }*/
  

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  if (!row_pointers) abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  png_destroy_write_struct(&png, &info);
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

//entradas: No tiene entradas por argumento, emplea una variable global
//funcionamiento: convierte la matriz de tipo png_byte a matriz de int y la almacena en una estructura llamada matriz
//salidas: puntero a estructura matriz
matriz* png_a_matriz(png_bytep* png, int filas, int columnas) {
  matriz* imagen = crearMatriz(filas, columnas);
  //matriz* imagen = malloc(sizeof(matriz));

  for(int y = 0; y < filas; y++) {
    png_bytep row = png[y];
    for(int x = 0; x < columnas; x++) {
      //png_bytep px = &(row[x * 4]);
      // Do something awesome for each pixel here...
      //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
      //printf("%3d", row[x]);
      imagen->matriz[y][x] = (int)png[y][x];
    }
    //printf("\n");
  }
  return imagen;
}

//entradas: puntero a estructura matriz que contiene la imagen luego del proceso pooling
//          entero que representa el umbral para clasificar el porcentaje de pixeles negros de la imagen
//funcionamiento: clasifica si la imagen es nearly black, de acuerdo a si su porcentaje de pixeles negros es mayor o menor al umbral
//salidas: No tiene salida, se muestra por pantalla si la imagen es nearly black o no
void clasificacion(matriz* imagen, int umbral, int bandera, int numeroHebras)
{
    //int pixeles_negros = 0;
    int filas, columnas;
    filas = 0;
    columnas = 0;    
    for (int i = 0; i < imagen->filas; i++)
    {
        for (int j = 0; j < imagen->columnas; j++)
        {
            if (imagen->matriz[i][j] == 0)
            {
                pthread_mutex_lock(&mutexPixeles);
                pixeles_negros++;
                pthread_mutex_unlock(&mutexPixeles);
            }
            
        }
        
    }

    if (height % 2 == 0)
    {
        filas = height/2;
    }
    else
    {
        filas = (height+1)/2;
    }

    if (width % 2 == 0)
    {
        columnas = width/2;
    }
    else
    {
        columnas = (width+1)/2;
    }

    if (contadorThreads == numeroHebras -1)
    {
        //printf("contador: %d, numhebras: %d\n", contadorThreads, numeroHebras);
        int total_pixeles =  filas * columnas;
        int porcentaje_negro = (pixeles_negros * 100)/total_pixeles;
        printf("porcentaje negro: %d\n", porcentaje_negro);

        if (porcentaje_negro >= umbral && bandera)
        {
            printf("la imagen es nearly black\n");
            return;
        }
        else if(bandera)
        {
            printf("la imagen no es nearly black\n");
            return;
        }    
    }
    else
    {
        pthread_mutex_lock(&mutexNHebras);
        contadorThreads++;
        pthread_mutex_unlock(&mutexNHebras);
    }
    
        

}