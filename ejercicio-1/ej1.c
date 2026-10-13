/*
Ejercicio 1

Cree un programa en C que muestre el ID de cada hilo dentro dos bloques paralelos distintos 
OpenMP y la cantidad de hilos que se crearon. En el primer bloque la cantidad de hilos se 
indicará mediante una “función de OpenMP” y en el segundo bloque mediante una “cláusula 
de OpenMP”. La cantidad de hilos debe ser suministrada por parámetro. En caso de no 
ingresarse un valor o que el valor sea menor a 2, ejecutar el bloque paralelo como si fuera 
uno secuencial.

Ejemplo de impresión por consola:
“Soy el hilo 1 de 8 hilos y pertenezco al primer bloque” 
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int n_hilos = 1;

    if (argc == 2) {
        int parametro = atoi(argv[1]);
        if (parametro >= 2) {
            n_hilos = parametro;
        }
    }

    printf("Iniciando programa. Se utilizaran %d hilo(s).\n\n", n_hilos);
    
    omp_set_num_threads(n_hilos);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int total = omp_get_num_threads();
        
        printf("Soy el hilo %d de %d hilos y pertenezco al primer bloque\n", id + 1, total);
    }

    printf("Fin del primer bloque\n");

    #pragma omp parallel num_threads(n_hilos)
    {
        int id = omp_get_thread_num();
        int total = omp_get_num_threads();
        
        printf("Soy el hilo %d de %d hilos y pertenezco al segundo bloque\n", id + 1, total);
    }

    printf("Fin del segundo bloque\n");

    return 0;
}