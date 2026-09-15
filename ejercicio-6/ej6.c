/*
    Ejercicio 6

    Modifique la solución del problema anterior agregando la condición de que el puente sólo
    acepta hasta 4 autos a la vez. Esta solución también debe ser con semáforos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// Semaforos globales sin nombre
sem_t sem_puente;
sem_t mut_norte;
sem_t mut_sur;
sem_t sem_capacidad; // Semaforo para limitar a 4 autos

// Contadores globales
int autos_norte = 0;
int autos_sur = 0;

void *hilo_norte(void *arg)
{
    long id = (long) arg; 

    // Espera aleatoria antes de llegar (hasta 30 segundos)
    int espera = rand() % 31;
    sleep(espera);
    printf("Norte %ld: LLEGUE al puente (espere %d segs).\n", id, espera);

    // Entrada Norte
    sem_wait(&mut_norte);
    autos_norte++;
    if (autos_norte == 1) {
        // Soy el primer auto del norte, bloqueo el puente para los del sur
        sem_wait(&sem_puente);
    }
    sem_post(&mut_norte);

    // Pido un "lugar" en el puente. Si ya hay 4, me bloqueo aca hasta que salga uno.
    sem_wait(&sem_capacidad);

    // Cruzando el puente (400ms = 400000 microsegundos)
    printf("Norte %ld: CRUZANDO...\n", id);
    usleep(400000); 

    // Ya cruce, devuelvo mi "lugar" en el puente para que entre otro.
    sem_post(&sem_capacidad);

    // Salida Norte
    sem_wait(&mut_norte);
    autos_norte--;
    printf("Norte %ld: SALI del puente. (Quedan %d cruzando)\n", id, autos_norte);
    if (autos_norte == 0) {
        // Fui el ultimo auto del norte, libero el puente para los del sur
        sem_post(&sem_puente);
    }
    sem_post(&mut_norte);

    pthread_exit((void *) 0);
}

void *hilo_sur(void *arg)
{
    long id = (long) arg;

    // Espera aleatoria antes de llegar (hasta 30 segundos)
    int espera = rand() % 31;
    sleep(espera);
    printf("Sur %ld: LLEGUE al puente (espere %d segs).\n", id, espera);

    // Entrada Sur
    sem_wait(&mut_sur);
    autos_sur++;
    if (autos_sur == 1) {
        // Soy el primer auto del sur, bloqueo el puente para los del norte
        sem_wait(&sem_puente);
    }
    sem_post(&mut_sur);

    // Pido un "lugar" en el puente.
    sem_wait(&sem_capacidad);

    // Cruzando el puente (400ms)
    printf("Sur %ld: CRUZANDO...\n", id);
    usleep(400000); 

    // Ya cruce, devuelvo mi "lugar".
    sem_post(&sem_capacidad);

    // Salida Sur
    sem_wait(&mut_sur);
    autos_sur--;
    printf("Sur %ld: SALI del puente. (Quedan %d cruzando)\n", id, autos_sur);
    if (autos_sur == 0) {
        // Fui el ultimo auto del sur, libero el puente para los del norte
        sem_post(&sem_puente);
    }
    sem_post(&mut_sur);

    pthread_exit((void *) 0);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Uso: %s <cantidad_autos_por_lado>\n", argv[0]);
        exit(1);
    }

    int n_autos = atoi(argv[1]);
    int retval;
    long i;
    
    srand(time(NULL));

    // Inicializacion de semaforos 
    if(sem_init(&sem_puente, 0, 1) == -1) perror("sem_init(sem_puente)");
    if(sem_init(&mut_norte, 0, 1) == -1) perror("sem_init(mut_norte)");
    if(sem_init(&mut_sur, 0, 1) == -1) perror("sem_init(mut_sur)");
    if(sem_init(&sem_capacidad, 0, 4) == -1) perror("sem_init(sem_capacidad)"); // NUEVO: Inicializo en 4

    printf("Comienzo de la prueba. %d autos de cada lado.\n", n_autos);

    // Como n_autos es dinamico por parametro
    pthread_t *hilos_n = (pthread_t *)malloc(n_autos * sizeof(pthread_t));
    pthread_t *hilos_s = (pthread_t *)malloc(n_autos * sizeof(pthread_t));

    // Creacion de hilos Norte
    for(i = 0; i < n_autos; ++i) {
        retval = pthread_create(&hilos_n[i], NULL, &hilo_norte, (void *) i);
        if(retval != 0) exit(1);
    }

    // Creacion de hilos Sur
    for(i = 0; i < n_autos; ++i) {
        retval = pthread_create(&hilos_s[i], NULL, &hilo_sur, (void *) i);
        if(retval != 0) exit(1);
    }

    // Esperar a que terminen
    for(i = 0; i < n_autos; ++i) {
        pthread_join(hilos_n[i], NULL);
        pthread_join(hilos_s[i], NULL);
    }

    // Limpieza
    sem_destroy(&sem_puente);
    sem_destroy(&mut_norte);
    sem_destroy(&mut_sur);
    sem_destroy(&sem_capacidad); 
    free(hilos_n);
    free(hilos_s);

    printf("Fin\n");
    return 0;
}