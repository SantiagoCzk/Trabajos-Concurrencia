/*
    Ejercicio 4

    Cree un programa llamado ej4.c el cual genere dos hilos. Uno sera el hilo productor y el
    otro sera el consumidor. EL hilo productor debe generar caracteres aleatorios y
    guardarlos en un buffer. El hilo consumidor tiene que leer los valores del buffer y
    mostrarlos por consola. Cuando se habla de buffer, se puede representar con un arreglo.
    Como detalle a tener en cuenta:
    a. Cuando el productor quiere generar un caracter y el buffer esta lleno.
    b. Cuando el consumidor quiere leer un caracter del buffer y el mismo esta vacio.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

const int MAX_ITEMS = 15;   // Cantidad total de items que vamos a producir/consumir para que el programa termine
const int MAX_BUFFER = 5;  // Tamanio del arreglo buffer
char *buffer;               // Arreglo dinamico para el buffer
int cont = 0;              // Cantidad actual de elementos
int i = 0;                 // Indice de entrada (Productor)
int j = 0;                // Indice de salida (Consumidor)

// Sincronizacion (Mutex y Variables de Condicion)
pthread_mutex_t mutex;
pthread_cond_t cond_productor;
pthread_cond_t cond_consumidor;

// Estructura para empaquetar los argumentos en las funciones
typedef struct {
    int id; // Identificador del hilo (0 para productor, 1 para consumidor)
} DatosHilo;


// Funcion Hilo Productor
void* productor(void *arg) {
    DatosHilo *datos = (DatosHilo*)arg;
    
    for (int k = 0; k < MAX_ITEMS; k++) {
        char caracter = 'A' + (rand() % 26); 

        pthread_mutex_lock(&mutex);

        // Si el buffer esta lleno, esperamos
        while (cont == MAX_BUFFER) {
            printf("Productor %d Buffer LLENO. Esperando...\n", datos->id);
            pthread_cond_wait(&cond_productor, &mutex);
        }

        buffer[i] = caracter;
        printf("Productor %d Inserto: '%c' en posicion %d\n", datos->id, caracter, i);
        
        i = (i + 1) % MAX_BUFFER;
        cont++;

        // Avisamos al consumidor que hay datos
        pthread_cond_signal(&cond_consumidor);

        pthread_mutex_unlock(&mutex);
        usleep(300000); // pausa para ver la consola
    }

    pthread_exit(NULL);
    return NULL;
}

// Funcion Hilo Consumidor
void* consumidor(void *arg) {
    DatosHilo *datos = (DatosHilo*)arg;
    
    for (int k = 0; k < MAX_ITEMS; k++) {
        pthread_mutex_lock(&mutex);

        // Si el buffer esta vacio, esperamos
        while (cont == 0) {
            printf("Consumidor %d Buffer VACÍO. Esperando...\n", datos->id);
            pthread_cond_wait(&cond_consumidor, &mutex);
        }

        char caracter = buffer[j];
        printf("Consumidor %d Consumio: '%c' de posicion %d\n", datos->id, caracter, j);
        
        j = (j + 1) % MAX_BUFFER;
        cont--;

        // Avisamos al productor que hay espacio
        pthread_cond_signal(&cond_productor);

        pthread_mutex_unlock(&mutex);
        usleep(500000); 
    }

    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char* argv[]) {

    srand(time(NULL)); // Inicializa la semilla para numeros aleatorios

    // Reservamos memoria dinamica 
    buffer = (char*)malloc(MAX_BUFFER * sizeof(char));
    pthread_t *arreglo_hilos = (pthread_t*)malloc(2 * sizeof(pthread_t));

    // Inicializamos el mutex y las variables de condición
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_productor, NULL);
    pthread_cond_init(&cond_consumidor, NULL);

    // Preparamos los datos y creamos el hilo productor
    DatosHilo *datos_prod = (DatosHilo*)malloc(sizeof(DatosHilo));
    datos_prod->id = 0;
    pthread_create(&arreglo_hilos[0], NULL, productor, datos_prod);

    // Preparamos los datos y creamos el hilo consumidor
    DatosHilo *datos_cons = (DatosHilo*)malloc(sizeof(DatosHilo));
    datos_cons->id = 1;
    pthread_create(&arreglo_hilos[1], NULL, consumidor, datos_cons);

    // Esperamos la finalizacion de los hilos
    for (int i = 0; i < 2; i++) {
        pthread_join(arreglo_hilos[i], NULL);
    }

    // Limpiamos las herramientas de sincronización
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_productor);
    pthread_cond_destroy(&cond_consumidor);

    // Liberamos la memoria dinamica asignada
    free(datos_prod);
    free(datos_cons);
    free(buffer);
    free(arreglo_hilos);

    printf("Fin.\n");

    return 0;
}