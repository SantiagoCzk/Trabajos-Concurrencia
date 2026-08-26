/*
    Ejercicio 3

    Escriba un programa llamado ej3.c que genere N cantidad de hilos, siendo N una
    constante (al menos mayor a 10). Cree un arreglo de enteros de N posiciones, el cual se
    va a inicializar con valores aleatorios. El proceso principal dispone de una variable
    global. Cada hilo tiene que sumarle a esa variable global, el valor correspondiente de su
    posicion en el arreglo. Por ejemplo si se esta ejecutando el hilo numero tres, usara el
    valor del arreglo que este en la posicion 3. Tenga en cuenta que los arreglos se inician en
    la posicion cero. El proceso debe esperar la finalizacion de todos los hilos y mostrar el
    valor resultante en la variable global.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

int contador = 0;   // Variable global compartida por todos los hilos
const int MAX = 11; // Tamaño del arreglo y cantidad de hilos

// Estructura para empaquetar los argumentos en las funciones
typedef struct {
    int *arreglo;   
    int indice;   
    int size;       
} DatosHilo;


// Función que ejecutará el hilo para inicializar el arreglo
void* inicializar_arreglo(void *arg) {
    DatosHilo *datos = (DatosHilo*)arg;
    for (int i = 0; i < datos->size; i++) {
        datos->arreglo[i] = rand() % 100;
    }
    pthread_exit(NULL);
    return NULL;
}

// Funcion que ejecuta cada hilo
void* sumar_valor(void *arg) {
    DatosHilo *datos = (DatosHilo*)arg;
    contador += datos->arreglo[datos->indice];
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char* argv[]) {

    srand(time(NULL)); // Inicializa la semilla para numeros aleatorios

    // Reservamos memoria dinamica para el arreglo de numeros
    int *arreglo_numeros = (int*)malloc(MAX * sizeof(int));

    // Reservamos memoria dinamica para el arreglo de identificadores de hilos
    pthread_t *arreglo_hilos = (pthread_t*)malloc(MAX * sizeof(pthread_t));

    // Creamos un hilo que inicializa el arreglo con valores aleatorios entre 0 y 99
    pthread_t hilo_inicializador;
    DatosHilo dato_init;
    dato_init.arreglo = arreglo_numeros;
    dato_init.size = MAX;

    pthread_create(&hilo_inicializador, NULL, inicializar_arreglo, &dato_init);

    // Espera a que el hilo termine para asegurar que el arreglo ya esté inicializado
    pthread_join(hilo_inicializador, NULL);

    // Creamos MAX hilos, cada uno suma el valor de arreglo[indice] a la variable global
    for (int i = 0; i < MAX; i++) {
        DatosHilo *datos = (DatosHilo*)malloc(sizeof(DatosHilo));
        datos->arreglo = arreglo_numeros;
        datos->indice = i;
        datos->size = MAX;
        pthread_create(&arreglo_hilos[i], NULL, sumar_valor, datos);
    }

    // Esperamos la finalizacion de todos los hilos sumadores
    for (int i = 0; i < MAX; i++) {
        pthread_join(arreglo_hilos[i], NULL);
    }

    // Mostramos el valor resultante de la variable global
    printf("Resultado: %d\n", contador);

    // Liberamos la memoria dinamica asignada
    free(arreglo_numeros);
    free(arreglo_hilos);

    return 0;
}