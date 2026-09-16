/*
    Ejercicio 3

    Implemente la solución del productor/consumidor con mutex y variable de condición.
    Utilice un buffer de 100 posiciones. Se debe crear 2 hilo productor y 2 hilos consumidores.
    Recordemos los detalles a tener en cuenta:

    a) Los productores escriben un carácter aleatorio en el buffer si el mismo no está lleno.
    b) Para el caso de los consumidores, tendremos dos funcionamientos distintos:
        i. El “consumidor 1” funcionará de la forma normal; Si el buffer no está vacío, leer un
        carácter del mismo.
        ii. El “consumidor 2” tendrá un leve cambio con respecto al anterior. Solo leerá un
        valor del buffer, si el mismo al menos tiene 10 caracteres sin consumir.
        iii. Cuando los hilos consumidores se encuentran bloqueados en una variable de
        condición, se debe despertar a todos los hilos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define TAM_BUFFER 100
#define NUM_PRODUCTORES 2
#define ITERACIONES_PROD 200
#define ITERACIONES_CONS 200

// Buffer circular y punteros de posicion
char buffer[TAM_BUFFER];
int in = 0;    // Indice donde el productor inserta
int out = 0;   // Indice donde el consumidor extrae
int count = 0; // Cantidad de elementos actuales en el buffer

// Recursos de sincronizacion POSIX
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable de condicion unica para alertar cambios en el estado del buffer
pthread_cond_t cond_buffer = PTHREAD_COND_INITIALIZER;

// Funcion para generar caracteres aleatorios (A-Z)
char generar_caracter_random() {
    return 'A' + (rand() % 26);
}

void* hilo_productor(void* arg) {
    long id = (long)arg;

    for (int i = 0; i < ITERACIONES_PROD; i++) {
        pthread_mutex_lock(&mutex);

        // a) Esperar si el buffer esta lleno
        while (count == TAM_BUFFER) {
            pthread_cond_wait(&cond_buffer, &mutex);
        }

        // Insertar caracter aleatorio en el buffer circular
        char item = generar_caracter_random();
        buffer[in] = item;
        in = (in + 1) % TAM_BUFFER;
        count++;

        printf("[PRODUCTOR %ld] Produjo: '%c' | Total en buffer: %d\n", id, item, count);

        // iii) Despertar a TODOS los hilos bloqueados
        pthread_cond_broadcast(&cond_buffer);

        pthread_mutex_unlock(&mutex);

        usleep(1000); // Pequeña pausa opcional
    }

    pthread_exit(NULL);
}

void* consumidor_1(void* arg) {
    (void)arg; // Silencia el warning de parametro no usado

    for (int i = 0; i < ITERACIONES_CONS; i++) {
        pthread_mutex_lock(&mutex);

        // b.i) Esperar si el buffer esta vacio
        while (count == 0) {
            pthread_cond_wait(&cond_buffer, &mutex);
        }

        // Leer caracter del buffer
        char item = buffer[out];
        out = (out + 1) % TAM_BUFFER;
        count--;

        printf("  -> [CONSUMIDOR 1] Consumio: '%c' | Total en buffer: %d\n", item, count);

        // iii) Despertar a TODOS los hilos bloqueados
        pthread_cond_broadcast(&cond_buffer);

        pthread_mutex_unlock(&mutex);

        usleep(1500);
    }

    pthread_exit(NULL);
}

void* consumidor_2(void* arg) {
    (void)arg; // Silencia el warning de parametro no usado

    for (int i = 0; i < ITERACIONES_CONS; i++) {
        pthread_mutex_lock(&mutex);

        // b.ii) Solo lee si al menos hay 10 caracteres sin consumir (count >= 10)
        while (count < 10) {
            pthread_cond_wait(&cond_buffer, &mutex);
        }

        // Leer caracter del buffer
        char item = buffer[out];
        out = (out + 1) % TAM_BUFFER;
        count--;

        printf("  ==> [CONSUMIDOR 2] Consumio: '%c' | Total en buffer: %d\n", item, count);

        // iii) Despertar a TODOS los hilos bloqueados
        pthread_cond_broadcast(&cond_buffer);

        pthread_mutex_unlock(&mutex);

        usleep(1500);
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    pthread_t productores[NUM_PRODUCTORES];
    pthread_t cons1, cons2;

    printf("Iniciando Productor/Consumidor (Buffer: %d posiciones)...\n\n", TAM_BUFFER);

    // Crear 2 hilos productores
    for (long i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_create(&productores[i], NULL, hilo_productor, (void*)(i + 1));
    }

    // Crear Consumidor 1 y Consumidor 2
    pthread_create(&cons1, NULL, consumidor_1, NULL);
    pthread_create(&cons2, NULL, consumidor_2, NULL);

    // Esperar a los productores
    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(productores[i], NULL);
    }

    // Esperar a los consumidores
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    printf("\n--- Finalizo la ejecucion de todos los hilos ---\n");

    // Limpieza de recursos POSIX
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_buffer);

    return 0;
}