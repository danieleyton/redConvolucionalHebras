#include <png.h>
#include <pthread.h>
#ifndef SHARED_H
#define SHARED_H

int width, height, pixeles_negros, contadorThreads;
png_byte color_type, bit_depth;
png_bytep *row_pointers;
buffer* miBuffer;
pthread_mutex_t mutexPixeles;
pthread_mutex_t mutex23, mutexNHebras;


#endif // !SHARED_H