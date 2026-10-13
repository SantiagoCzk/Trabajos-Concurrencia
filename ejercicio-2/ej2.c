/*
Ejercicio 2

Genere un programa en C que reparta la tarea de un bloque paralelo “for” (100 iteraciones) 
de forma dinámica en grupos de 15 iteraciones. Cada iteración debe mostrar por consola el 
número de hilo y número de iteración. Analice los resultados obtenidos por consola.
*/

#include <stdio.h>
#include <omp.h>

int main(void) {
    int i;
    int iteraciones = 100;

    printf("Iniciando bucle paralelo (100 iteraciones, dynamic, 15)\n\n");

    #pragma omp parallel for schedule(dynamic, 15)
    for (i = 0; i < iteraciones; i++) {
        int id = omp_get_thread_num();
        printf("Hilo %d ejecutando la iteracion %d\n", id + 1, i);
    }

    printf("Fin del bucle paralelo.\n");
    return 0;
}