/*
    Ejercicio 4

    Cree un programa en C que genere 6 hilos de tipo “h1”, 6 hilos de tipo “h2” y otros 6 hilos
    de tipo “h3”. Todos los hilos se ejecutarán de forma concurrente. Por otro lado se tienen dos
    recursos compartidos, A y B. Los hilos h1 utilizarán el recurso A, los hilos h2 utilizarán el
    recurso B y los hilos h3 utilizarán simultáneamente los recursos A y B. Tanto el recurso A
    como el B deben ser usados en exclusión mutua, esto es, solo un hilo puede acceder de
    forma simultánea al recurso. Por tanto, un hilo “h1” puede acceder al recurso A mientras un
    hilo “h2” está accediendo al recurso B. Sin embargo, cuando un hilo “h3” está accediendo a
    los recursos A y B, los hilos “h1” y “h2” no podrán acceder respectivamente a sus recursos.
    Cuando se habla de “utilizar el recurso”, es simplemente modificar su valor y mostrarlo por
    consola.
    Ejemplo:

    int A; // En algun lugar del programa se define el recurso 
    ...
    A += 10; // Ejemplo de utilizar el recurso
    printf(A);
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Recursos compartidos
int recurso_A = 0;
int recurso_B = 0;

// Banderas de estado de los recursos (0 = Libre, 1 = Ocupado)
int ocupado_A = 0;
int ocupado_B = 0;

// Inicialización estática del Mutex y la Variable de Condición
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Hilo H1: Utiliza SOLO el recurso A
void* hilo_h1(void* arg) {
    int id = (long)arg;

    // Solicita el recurso A
    pthread_mutex_lock(&mut);
    while (ocupado_A == 1) {
        pthread_cond_wait(&cond, &mut); // Si A está ocupado, el hilo duerme
    }
    ocupado_A = 1; // Reserva el recurso A
    pthread_mutex_unlock(&mut);

    // Utiliza el recurso A (Región Crítica)
    recurso_A += 10;
    printf("[H1 - id: %d] Modificó Recurso A -> Valor actual A: %d\n", id, recurso_A);

    // Libera el recurso A
    pthread_mutex_lock(&mut);
    ocupado_A = 0; // Libero A
    pthread_cond_broadcast(&cond); // Despierta a todos los que estén esperando
    pthread_mutex_unlock(&mut);

    pthread_exit(NULL);
}

// Hilo H2: Utiliza SOLO el recurso B
void* hilo_h2(void* arg) {
    int id = (long)arg;

    // Solicita el recurso B
    pthread_mutex_lock(&mut);
    while (ocupado_B == 1) {
        pthread_cond_wait(&cond, &mut); // Si B está ocupado, el hilo duerme
    }
    ocupado_B = 1; // Reserva el recurso B
    pthread_mutex_unlock(&mut);

    // Utilizar el recurso B (Región Crítica)
    recurso_B += 20;
    printf("[H2 - id: %d] Modificó Recurso B -> Valor actual B: %d\n", id, recurso_B);

    // Libera el recurso B
    pthread_mutex_lock(&mut);
    ocupado_B = 0; // Libera B
    pthread_cond_broadcast(&cond); // Despierta a todos
    pthread_mutex_unlock(&mut);

    pthread_exit(NULL);
}

// Hilo H3: Utiliza los recursos A y B SIMULTÁNEAMENTE
void* hilo_h3(void* arg) {
    int id = (long)arg;

    // Solicita los recursos A y B
    pthread_mutex_lock(&mut);
    // h3 solo puede avanzar si TANTO A como B están libres
    while (ocupado_A == 1 || ocupado_B == 1) {
        pthread_cond_wait(&cond, &mut); // Si alguno está ocupado, el hilo duerme
    }
    ocupado_A = 1; // Reserva A
    ocupado_B = 1; // Reserva B
    pthread_mutex_unlock(&mut);

    // Utiliza ambos recursos (Región Crítica)
    recurso_A += 50;
    recurso_B += 50;
    printf("[H3 - id: %d] Modificó A y B -> Valores actuales A: %d, B: %d\n", id, recurso_A, recurso_B);

    // Libera los recursos A y B
    pthread_mutex_lock(&mut);
    ocupado_A = 0;
    ocupado_B = 0;
    pthread_cond_broadcast(&cond); // Despierta a todos
    pthread_mutex_unlock(&mut);

    pthread_exit(NULL);
}

int main() {
    pthread_t hilos_1[6], hilos_2[6], hilos_3[6];

    // Crear los hilos (6 de cada tipo)
    for (long i = 0; i < 6; i++) {
        pthread_create(&hilos_1[i], NULL, hilo_h1,(void *) i + 1); 
        pthread_create(&hilos_2[i], NULL, hilo_h2,(void *) i + 1);
        pthread_create(&hilos_3[i], NULL, hilo_h3,(void *) i + 1);
    }

    // Esperar a que todos terminen
    for (long i = 0; i < 6; i++) {
        pthread_join(hilos_1[i], NULL);
        pthread_join(hilos_2[i], NULL);
        pthread_join(hilos_3[i], NULL);
    }

    printf("\nEjecucion finalizada\n");

    return 0;
}