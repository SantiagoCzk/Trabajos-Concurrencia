/*
Ejercicio 5

Cree un programa en C que contenga un bloque paralelo y se ejecute con 4 hilos, donde cada 
hilo muestre su ID por consola en exclusión mutua, para lo cual se debe usar la directiva 
“critical”. Luego de que cada hilo muestre su ID, generar una barrera con la directiva 
“barrier” y mostrar un mensaje que termino la ejecución.
*/

#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel num_threads(4)
    {
        #pragma omp critical
        printf("Soy el hilo %d\n", omp_get_thread_num());

        #pragma omp barrier

        #pragma omp single
        printf("Ejecucion terminada\n");
    }
    return 0;
}