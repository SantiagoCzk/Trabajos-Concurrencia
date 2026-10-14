/*
Ejercicio 3

Cree un programa en C que realice dos funciones de manera concurrente. Estas funciones se 
llamarán “funcion_tiempo_1” y “funcion_tiempo_2”. Cada función hará un sleep(1) y 
sleep(2) respectivamente. Para esto debe usar la directiva “sections”. Medir el tiempo de 
ejecución de todo el programa con la función “omp_get_wtime()”, y mostrarlo por consola. 
Cada función genera un double, que al restarlos, se obtiene el tiempo de duración. Ejemplo:

double inicio, fin;
inicio = omp_get_wtime();
... Procesamiento ...
fin = omp_get_wtime();
resultado = fin – inicio;
*/

#include <stdio.h>
#include <unistd.h>
#include <omp.h>

double funcion_tiempo_1(void) {
    double inicio = omp_get_wtime();
    sleep(1);
    double fin = omp_get_wtime();
    return fin - inicio;
}

double funcion_tiempo_2(void) {
    double inicio = omp_get_wtime();
    sleep(2);
    double fin = omp_get_wtime();
    return fin - inicio;
}

int main(void) {
    double t1 = 0, t2 = 0;

    double inicio = omp_get_wtime();

    #pragma omp parallel sections
    {
        #pragma omp section
        t1 = funcion_tiempo_1();

        #pragma omp section
        t2 = funcion_tiempo_2();
    }

    double fin = omp_get_wtime();

    printf("funcion_tiempo_1: %.2f s\n", t1);
    printf("funcion_tiempo_2: %.2f s\n", t2);
    printf("Tiempo total del programa: %.2f s\n", fin - inicio);

    return 0;
}