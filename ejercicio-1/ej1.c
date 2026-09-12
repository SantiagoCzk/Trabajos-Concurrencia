/* 
    Ejercicio 1

    Genere una matriz cuadrada de tamaño N definida por el usuario. Cree N hilos, donde cada
    hilo muestre por consola los valores de una fila de la matriz. Se debe respetar el orden de las
    filas al momento de visualizar los valores por consola. El proceso principal debe esperar que
    todos los hilos terminen su ejecución.
    Ejemplo:
    Hilo1: Print Fila1
    Hilo2: Print Fila2
    ...
    HiloN: Print FilaN
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variables globales compartidas
int **matriz;
int N;
int turno_fila = 0; // Controla qué hilo tiene permiso para imprimir

// Inicialización estática del Mutex y la Variable de Condición
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Función que ejecutará cada hilo
void* imprimir_fila(void* arg) {
    int mi_fila = *(int*)arg;

    // Se adquiere el lock del mutex para entrar a la región crítica
    pthread_mutex_lock(&mut);

    // Verificamos si es el turno del hilo utilizando un ciclo while
    while (turno_fila != mi_fila) {
        // Si no es el turno del hilo, se bloquea y libera el mutex temporalmente
        pthread_cond_wait(&cond, &mut);
    }

    // El hilo imprime la fila de la matriz
    printf("Hilo %d: Print Fila %d -> ", mi_fila + 1, mi_fila + 1);
    for (int j = 0; j < N; j++) {
        printf("%d ", matriz[mi_fila][j]);
    }
    printf("\n");

    // Pasa el turno a la siguiente fila
    turno_fila++;

    // Despierta a TODOS los hilos bloqueados para que revisen si es su turno
    pthread_cond_broadcast(&cond);

    // Libera el lock del mutex para que el siguiente hilo pueda actuar[cite: 10]
    pthread_mutex_unlock(&mut);

    pthread_exit(NULL); // Termina la ejecución del hilo
}

int main(int argc, char *argv[]) {
    // Verificamos el argumento recibido
    if (argc != 2) {
        printf("Uso: %s <tamaño_matriz_N>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    if (N <= 0) {
        printf("N debe ser un entero positivo.\n");
        return 1;
    }

    // Reserva de memoria e inicialización de la matriz cuadrada
    matriz = (int**)malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        matriz[i] = (int*)malloc(N * sizeof(int));
        for (int j = 0; j < N; j++) {
            matriz[i][j] = (i * 10) + j; //Valores genéricos de relleno
        }
    }

    // Declaramos el arreglo de hilos que van a trabajar sobre la matriz
    pthread_t hilos[N];
    int ids_filas[N];

    // Creación de los N hilos
    for (int i = 0; i < N; i++) {
        ids_filas[i] = i; // Guardamos el ID que corresponde a la fila
        pthread_create(&hilos[i], NULL, imprimir_fila, &ids_filas[i]);
    }

    // El proceso principal espera a que todos los hilos terminen
    for (int i = 0; i < N; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Limpieza de la memoria dinámica
    for (int i = 0; i < N; i++) {
        free(matriz[i]);
    }
    free(matriz);

    return 0;
}