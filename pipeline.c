#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <asm/byteorder.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include "funciones.h"
#include <semaphore.h>
#include "shared.h"

#define READ 0
#define WRITE 1

sem_t lleno;
sem_t vacio;
pthread_barrier_t barrera1;
pthread_barrier_t barrera2;
pthread_barrier_t barrera3;
pthread_barrier_t barrera4;
pthread_barrier_t barrera5;
pthread_mutex_t mutex;
pthread_mutex_t mutexP;
pthread_mutex_t mutexC;
matriz* resultado;

void recibirParametros(int argc, char *argv[], int* numeroImagenes, char* nombreMascara, int* umbral, int* bandera, int* numeroHebras, int* bufferSize) {
  
  int c;
  *bandera = 0;

  while ((c = getopt(argc, argv, "c:m:n:bh:t:")) != -1) {
    switch (c) {
      case 'c':
        sscanf(optarg, "%d", numeroImagenes);
        if (numeroImagenes < 0)
        {
            printf("la cantidad de imagenes a leer no puede ser negativa\n");
            exit(1);
        }
        break;      
      case 'm':
        sscanf(optarg, "%s", nombreMascara);
        break;
      case 'n':
        sscanf(optarg, "%d", umbral);
        if (umbral < 0)
        {
          printf("El valor del umbral no puede ser negativo\n");
          exit(1);
        }
        
        break;
      case 'b':
        *bandera = 1;
        break;
      case 'h':
        sscanf(optarg, "%d", numeroHebras);
        if (numeroHebras < 0)
        {
          printf("La cantidad de hebras no puede ser negativa\n");
          exit(1);
        }
        
        break;
      case 't':
        sscanf(optarg, "%d", bufferSize);
        if (bufferSize < 0)
        {
          printf("El tamaño del buffer no puede ser negativo\n");
          exit(1);
        }
        
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

  if (argc < 6)
  {
    printf("La cantidad de argumentos es menor a 5\n");
  }
  

  for (int i = optind; i < argc; i++)
  {
    printf("Non-option argument %s\n", argv[i]);
  }
  

}

void inicializarBuffer(int bufferSize)
{
  miBuffer = (buffer*)malloc(sizeof(buffer));

  miBuffer->filas = (png_bytep*)malloc(sizeof(png_bytep) * bufferSize);

  miBuffer->ultimo = 0;
  miBuffer->size = bufferSize;
  miBuffer->pos = 0;
  
  for (int  i = 0; i < bufferSize; i++)
  {
    miBuffer->filas[i] = (png_byte*)malloc(sizeof(png_byte) * width);
  }

  for (int i = 0; i < bufferSize; i++)
  {
    for (int j = 0; j < width; j++)
    {
      miBuffer->filas[i][j] = 0;
    }
    
  }
  
  
}

pthread_t* crearHebras(int numeroHebras)
{
  pthread_t* hebras = malloc(sizeof(pthread_t)*numeroHebras);

  return hebras;
}

void* hebraProductora(void* offset)
{
  int indice = 0;  
  for(int n = 0; n < height; n++){

    for (int i = 0; i < miBuffer->size; i++)
    {
      indice = *(int*)offset + i;
      sem_wait(&vacio);
      miBuffer->filas[i] = row_pointers[n];
      sem_post(&lleno);
      n++;
      /*if (n >= height)
      {
        break;
      }*/
      
    }
    miBuffer->pos = n;
  *(int*)offset = *(int*)offset + indice;
  }
}

png_bytep* recogerFilas(int numFilas, int* posicion)
{
  png_bytep* aux = malloc(sizeof(png_bytep)*numFilas);
  for (int i = 0; i < numFilas; i++)
  {
    aux[i] = (png_byte*)malloc(sizeof(png_byte)*width);
  }

  *posicion = miBuffer->pos;
  
  for (int i = miBuffer->ultimo; i < miBuffer->size; i++)
  {
    sem_wait(&lleno);
    /*for(int j = 0; j < width; j++){
      printf("sip %d %d\n", i, j);
      printf("#%3d#", miBuffer->filas[i][j]);
    }
    printf("\n");*/
    aux[i] = miBuffer->filas[i];
    //miBuffer->filas[i] = NULL
    sem_post(&vacio);
  }

  return aux;
   
}

void ordenarImagen(int pos, matriz* porcionImagen)
{  
  //printf("\n\n\n pos: %d \n\n\n", pos);
  for (int i = 0; i < porcionImagen->filas; i++)
  {
    for (int j = 0; j < porcionImagen->columnas; j++)
    {
      //imagenFinal->matriz[i + pos][j] = porcionImagen->matriz[i][j];
      pthread_mutex_lock(&mutex);
      //row_pointers[i + pos][j] = 0;
      int moduloPosicion = (i + pos)%resultado->filas;
      resultado->matriz[moduloPosicion][j] = porcionImagen->matriz[i][j];
      pthread_mutex_unlock(&mutex);
    }
    
    //printf("%d\n", i+pos);
    //imagenFinal[i + pos] = porcionImagen[i];
  }  
}

void* hebraConsumidora(void* args)
{
  int posicion = 0;
  //recoger porcion de imagen

  png_bytep* porcionImagen = recogerFilas(((argumentos*)args)->numFilas, &posicion);  
  
  pthread_barrier_wait(&barrera1);
  matriz* mat = png_a_matriz(porcionImagen, ((argumentos*)args)->numFilas, width);
  pthread_barrier_wait(&barrera2);
  //aplicar convolucion
  //matriz* filtro = leerFiltro(((argumentos*)args)->nombreFiltro);
  matriz* conv = convolucion(mat, ((argumentos*)args)->filtro);
  pthread_barrier_wait(&barrera3);  
  //aplicar rectificacion
  rectificacion(conv);
  pthread_barrier_wait(&barrera4);
  //aplicar pooling
  matriz* poolng = pooling(conv);
  pthread_barrier_wait(&barrera5);
  //clasificacion
  clasificacion(poolng, ((argumentos*)args)->umbral, ((argumentos*)args)->bandera, ((argumentos*)args)->numeroHebras);
  pthread_barrier_wait(&barrera1);
  ordenarImagen(posicion, poolng);
  pthread_barrier_wait(&barrera1);
  //free porcionIMagen
}

void pipeline(int numImagenes, int numeroHebras, int bufferSize, argumentos* args)
{
  pthread_t* hebras = crearHebras(numeroHebras);
  for (int i = 0; i < numeroHebras; i++)
  {
    pthread_mutex_init(&mutexC, NULL);
  }
  
  pthread_t hebraProduc;
  pthread_barrier_init(&barrera1, NULL, numeroHebras);
  pthread_barrier_init(&barrera2, NULL, numeroHebras);
  pthread_barrier_init(&barrera3, NULL, numeroHebras);
  pthread_barrier_init(&barrera4, NULL, numeroHebras);
  pthread_barrier_init(&barrera5, NULL, numeroHebras);

  int numFilas = height/numeroHebras;
  args->numFilas = numFilas;
  int offset = 0;

  while (numImagenes > 0)
  {
    /*args->resultado = malloc(sizeof(png_bytep)*height);
    for (int i = 0; i < height; i++)
    {
      args->resultado[i] = malloc(sizeof(png_byte)*width);
    }*/
    args->resultado = crearMatriz(height, width);
    //hebraProductora(numImagenes, &offset);
    //char[2] sNumImagen;
    //sprintf(sNumImagen, "%d", numImagenes)
    pthread_create(&hebraProduc, NULL, hebraProductora, (void*)&offset);
    for (int i = 0; i < numeroHebras; i++)
    {
      pthread_create(&hebras[i], NULL, hebraConsumidora, (void*)args);
    }

    //pthread_barrier_wait(&barrera1);
    for (int i = 0; i < numeroHebras; i++)
    {
      pthread_join(hebras[i], NULL);
    }
    numImagenes = numImagenes - 1;
    //casteo_a_pngByte(((argumentos*)args)->resultado, row_pointers);
    write_png_file("salida1.png", resultado);
    //write_png_file("salida1.png", row_pointers);
    /*for (int i = 0; i < ((argumentos*)args)->resultado->filas; i++)
    {
      for (int j = 0; j < ((argumentos*)args)->resultado->columnas; j++)
      {
        printf("%3d", ((argumentos*)args)->resultado->matriz[i][j]);
      }
      printf("\n");
    }*/
    
    
  }
  
}

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char *argv[]) {
    char nombreMascara[20];
    int umbral, bandera, numeroImagenes, numeroHebras, bufferSize; 
    contadorThreads = 0;   
    pthread_t pth1;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutexP, NULL);
    pthread_mutex_init(&mutexPixeles, NULL);
    //pthread_mutex_init(&mutexC, NULL);
    recibirParametros(argc, argv, &numeroImagenes, nombreMascara, &umbral, &bandera, &numeroHebras, &bufferSize);
    sem_init(&lleno, 0, 0);
    sem_init(&vacio, 0, bufferSize);
    char fdRead[100];
    char sNumeroImagenes[3];
    char sUmbral[4];
    char sBandera[2];
    //sprintf(fdRead, "%d", pipes[READ]);
    sprintf(sNumeroImagenes, "%d", numeroImagenes);
    sprintf(sUmbral, "%d", umbral);
    sprintf(sBandera, "%d", bandera);
    //char* argumentos[7] = {"./read", fdRead, sNumeroImagenes, nombreMascara, sUmbral, sBandera, NULL};
    inicializarBuffer(bufferSize);
    argumentos* args = malloc(sizeof(argumentos));
    matriz* filtro = leerFiltro(nombreMascara);
    args->filtro = filtro;
    args->umbral = umbral;
    args->bandera = bandera;
    args->numeroHebras = numeroHebras;
    pthread_create(&pth1, NULL, leer_archivo_png, (void*)"imagen_1.png");    
    pthread_join(pth1, NULL);
    resultado = crearMatrizPooling(height, width);    

    pipeline(numeroImagenes, numeroHebras, bufferSize, args);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexP);
    pthread_mutex_destroy(&mutexC);
    
    return 0;
}