CC=gcc

all: pipeline

#pipeline: read.c convolucion.o rectificacion pooling clasificacion write.o pipeline.c funciones.h
#	$(CC) pipeline.c read.c convolucion.o write.o -o pipeline -pthread -lpng
pipeline: funciones.o  pipeline.c
	$(CC) pipeline.c funciones.c -o pipeline -pthread -lpng
#read: read.c
#	$(CC) read.c -o read -lpng

#convolucion: convolucion.c funciones.h
#	$(CC) convolucion.c -o convolucion

#rectificacion: rectificacion.c
#	$(CC) rectificacion.c -o rectificacion

#pooling: pooling.c
#	$(CC) pooling.c -o pooling

#clasificacion: clasificacion.c
#	$(CC) clasificacion.c -o clasificacion

#write: write.c
#	$(CC) write.c -o write -lpng

funciones: funciones.o funciones.h
	$(CC) -o funciones funciones.o -lpng

#clean:
#	rm -rf convolucion rectificacion pooling clasificacion write pipeline