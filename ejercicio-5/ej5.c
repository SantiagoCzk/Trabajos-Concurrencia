/*
    Ejercicio 5

    Resolver el problema del puente de una sola vía con semáforos. Los autos que vienen del
    norte y del sur se encuentran con un puente de una sola vía. Los autos que van en el mismo
    sentido pueden atravesar el puente en forma simultánea. En cambio, si se encuentran dos
    autos en sentidos opuestos, sólo uno puede atravesar el puente, mientras el otro debe esperar.
    En la espera se puede formar una fila de autos. Mientras hayan autos cruzando en un
    sentidos, pueden seguir entrando autos al puente en el mismo sentido.
    Desarrollar un programa que reciba la cantidad de autos como parámetro y cree esta
    cantidad de autos a cada lado del puente. Cada auto estará representado por un hilo que pasa
    un tiempo aleatorio en el lado creado y luego intenta cruzar el puente. Una vez que sale del
    otro lado termina su ejecución. La espera aleatoria será de hasta 30 segundos. El cruce del
    puente demanda un tiempo fijo de 400ms.
    Utilizar impresiones a pantalla que muestren claramente toda la simulación.
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

    // Cruzando el puente (400ms = 400000 microsegundos)
    printf("Norte %ld: CRUZANDO...\n", id);
    usleep(400000); 

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

    // Cruzando el puente (400ms)
    printf("Sur %ld: CRUZANDO...\n", id);
    usleep(400000); 

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
    free(hilos_n);
    free(hilos_s);

    printf("Fin\n");
    return 0;
}