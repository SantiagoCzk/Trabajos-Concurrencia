/*
    Ejercicio 1
    
    Escriba un programa llamado ej1.c que tenga un único contador y tres hilos para 
    modificar su valor. Uno de esos hilos, incrementa el contador en 2 en cada iteración y 
    muestra su valor en pantalla, mientras que los dos hilos restantes, lo decrementa en 1 y 
    muestra su valor en pantalla. Ejemplo “Hilo 1 incremento el contador al valor de 25”. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variable global compartida por todos los hilos
int contador = 0;

// Función que ejecutará el hilo incrementador
void* hilo_incrementador(void* arg) {
    int id_hilo = *(int*)arg; // Casteamos el argumento a entero
    
    for (int i = 0; i < 5; i++) {
        contador += 2;
        printf("Hilo %d incremento el contador al valor de %d\n", id_hilo, contador);
    }
    
    pthread_exit(NULL); // Termina la ejecución del hilo
}

// Función que ejecutarán los hilos decrementadores
void* hilo_decrementador(void* arg) {
    int id_hilo = *(int*)arg;
    
    for (int i = 0; i < 5; i++) {
        contador -= 1;
        printf("Hilo %d decremento el contador al valor de %d\n", id_hilo, contador);
    }
    
    pthread_exit(NULL); // Termina la ejecución del hilo
}

int main() {
    // Arreglo para almacenar los identificadores de los hilos
    pthread_t hilos[3];
    
    // Variables para identificar visualmente a cada hilo
    int id1 = 1;
    int id2 = 2;
    int id3 = 3;

    printf("Iniciando programa principal. Contador inicial: %d\n\n", contador);

    // Hilo 1: Incrementa
    pthread_create(&hilos[0], NULL, hilo_incrementador, &id1);
    
    // Hilos 2 y 3: Decrementan
    pthread_create(&hilos[1], NULL, hilo_decrementador, &id2);
    pthread_create(&hilos[2], NULL, hilo_decrementador, &id3);

    // El hilo principal espera a que terminen los 3 hilos creados
    pthread_join(hilos[0], NULL);
    pthread_join(hilos[1], NULL);
    pthread_join(hilos[2], NULL);

    printf("\nTodos los hilos han terminado. Valor final del contador: %d\n", contador);

    return 0;
}