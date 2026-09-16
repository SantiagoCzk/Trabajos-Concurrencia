/*
    Ejercicio 2
    
    Cree un programa en C que genere 20 hilos de tipo “sumadores” y 20 hilos de tipo
    “restadores”. Los hilos “sumadores” sumarán a una variable global entera, mientras que los
    hilos “restadores” restarán a esa variable global. Los hilos deben realizar dicha operación
    unas 500 veces. Mostrar por consola la operación realizada y su resultado. Se incluye la
    restricción de que el valor de la variable global siempre debe ser mayor que 0 (cero). El
    proceso principal debe esperar que todos los hilos terminen su ejecución.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_HILOS_SUM 20
#define NUM_HILOS_RES 20
#define ITERACIONES 500

//Inicializamos en 1 para que los restadores puedan operar de entrada si la condicion lo permite
int valor_global = 1;

// Mutex para la exclusion mutua sobre valor_global
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable de condicion para pausar a los restadores cuando valor_global <= 0
pthread_cond_t cond_mayor_cero = PTHREAD_COND_INITIALIZER;

void* hilo_sumador(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < ITERACIONES; i++) {
        pthread_mutex_lock(&mutex);

        valor_global++;
        printf("[SUMADOR %ld] Sumo 1 -> Nuevo valor: %d\n", id, valor_global);

        // Notificamos a los hilos restadores que el valor subió
        pthread_cond_signal(&cond_mayor_cero);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

void* hilo_restador(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < ITERACIONES; i++) {
        pthread_mutex_lock(&mutex);

        // RESTRICCIÓN: El valor debe ser SIEMPRE mayor a 0 (valor_global > 0).
        // Si valor_global - 1 da 0 o menos, el hilo DEBE esperar.
        // Se usa 'while' en lugar de 'if' para evitar despertares espurios (spurious wakeups).
        while (valor_global <= 1) {
            pthread_cond_wait(&cond_mayor_cero, &mutex);
        }

        valor_global--;
        printf("[RESTADOR %ld] Resto 1 -> Nuevo valor: %d\n", id, valor_global);

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t sumadores[NUM_HILOS_SUM];
    pthread_t restadores[NUM_HILOS_RES];

    printf("Iniciando programa... Valor inicial: %d\n\n", valor_global);

    // Creacion de los hilos sumadores
    for (long i = 0; i < NUM_HILOS_SUM; i++) {
        if (pthread_create(&sumadores[i], NULL, hilo_sumador, (void*)i) != 0) {
            perror("Error al crear hilo sumador");
            exit(EXIT_FAILURE);
        }
    }

    // Creacion de los hilos restadores
    for (long i = 0; i < NUM_HILOS_RES; i++) {
        if (pthread_create(&restadores[i], NULL, hilo_restador, (void*)i) != 0) {
            perror("Error al crear hilo restador");
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a que terminen todos los hilos sumadores
    for (int i = 0; i < NUM_HILOS_SUM; i++) {
        pthread_join(sumadores[i], NULL);
    }

    // Esperar a que terminen todos los hilos restadores
    for (int i = 0; i < NUM_HILOS_RES; i++) {
        pthread_join(restadores[i], NULL);
    }

    printf("\n--- Todos los hilos finalizados ---");
    printf("\nValor final de la variable global: %d\n", valor_global);

    // Destruccion de recursos POSIX
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_mayor_cero);

    return 0;
}