CC=gcc

all: pipeline

pipeline: read convolucion rectificacion pooling clasificacion write pipeline.c
	$(CC) pipeline.c -o pipeline

read: read.c
	$(CC) read.c -o read -lpng

convolucion: convolucion.c
	$(CC) convolucion.c -o convolucion

rectificacion: rectificacion.c
	$(CC) rectificacion.c -o rectificacion

pooling: pooling.c
	$(CC) pooling.c -o pooling

clasificacion: clasificacion.c
	$(CC) clasificacion.c -o clasificacion

write: write.c
	$(CC) write.c -o write -lpng

clean:
	rm -rf read convolucion rectificacion pooling clasificacion write pipeline