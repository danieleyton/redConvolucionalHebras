#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
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

//entradas: argc la cantidad de argumentos leidos desde stdio, argv un arreglo de strings con los argumentos leidos
//funcionamiento: es la funcion main del archivo, hace todas las llamadas a funciones necesarias
//salidas: un entero con valo 0
int main(int argc, char const *argv[])
{
    //matriz* a, *b;
    char nombreFiltro[20];
    strcpy(nombreFiltro, argv[4]);
    matriz* filtro = leerFiltro(nombreFiltro);

    //matriz* resultado = convolucion(a, b);
    //char mensaje[10];
    //matriz* imagen;
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    //int imagen[filas][columnas];
    matriz* imagen = crearMatriz(filas, columnas);
    int pipes[2];
    pipe(pipes);
    //int leido = read(atoi(argv[1]), imagen, atoi(argv[2]));
    //printf("filas imagen: %d, columnas imagen: %d\n", imagen->filas, imagen->columnas);
    //printf("deberia haber leido: %d\n", atoi(argv[2]));
    int leido;

    for (int i = 0; i < filas; i++)
    {
        for (int j = 0; j < columnas; j++)
        {
            leido = read(atoi(argv[1]), &imagen->matriz[i][j], sizeof(int));
        }
        
    }

    //printf("asgdfa: %d\n", imagen->matriz[0][0]);
       
    //printf("leido: %d\n", leido);
    
    matriz* resultado;
    if (leido > 0)
    {
        //printf("soy el proceso convolucion y lei: %s\n", mensaje);
        resultado = convolucion(imagen, filtro);
        //printf("filas resultado: %d, columnas resultado: %d\n", resultado->filas, resultado->columnas);
        /*for (int i = 0; i < resultado->filas; i++)
        {
            for (int j = 0; j < resultado->columnas; j++)
            {
                printf("%3d", resultado->matriz[i][j]);
            }

            printf("\n");
            
        }*/
        
    }
    else
    {
        printf("size matriz*: %d\n", sizeof(matriz*));
        printf("error con el read: %d\n", leido);
    }
    

    int pid = fork();
    if (pid == 0)
    {        
        char fdRead[10];
        char sHeight[5];
        char sWidth[5];
        char sUmbral[4];
        char sBandera[2];
        sprintf(fdRead, "%d", pipes[0]);
        sprintf(sHeight, "%d", resultado->filas);
        sprintf(sWidth, "%d", resultado->columnas);
        sprintf(sUmbral, "%d", atoi(argv[5]));
        sprintf(sBandera, "%d", atoi(argv[6]));
        char* argumentos[7] = {"./rectificacion", fdRead, sHeight, sWidth, sUmbral, sBandera, NULL};
        close(pipes[1]);
        execv("./rectificacion", argumentos);
        printf("el execv fallo");
    }
    else
    {
        close(pipes[0]);
        //write(pipes[1], "olaola", 10);
        for (int i = 0; i < resultado->filas; i++)
        {
            for (int j = 0; j < resultado->columnas; j++)
            {
                write(pipes[1], &resultado->matriz[i][j], sizeof(int));
            }
            
        }
        
    }
    int status;
    wait(&status);
    return 0;
}
