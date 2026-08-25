/*
    Ejercicio 2
    
    Cree un programa llamado ej2.c que defina un arreglo de enteros. Genere un hilo que 
    inicialice el arreglo con valores aleatorios. Al finalizar, el proceso principal debe mostrar 
    por consola los valores del arreglo de forma secuencial. El tamaño del arreglo será 
    suministrado por medio de un argumento al programa. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Estructura para empaquetar los argumentos en las funciones
typedef struct {
    int *arreglo;
    int size;
} DatosHilo;

// Función que ejecutará el hilo para inicializar el arreglo
void* inicializar_arreglo(void *arg) {
    // Se castea el argumento genérico
    DatosHilo *datos = (DatosHilo*)arg;

    // Accedemos a los valores empaquetados
    for (int i = 0; i < datos->size; i++) {
        datos->arreglo[i] = rand() % 100;   // Inicializa con valores aleatorios entre 0 y 99
    }

    pthread_exit(NULL); 
}

int main(int argc, char* argv[]) {
    
    srand(time(NULL)); // Inicializa la semilla para los números aleatorios (necesario)

    // Verificamos el ingreso del tamaño del arreglo
    if (argc != 2) {
        printf("Uso: %s <tamaño del arreglo>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]); // Convertimos el argumento recibido a entero

    if (size <= 0) {
        printf("El tamaño del arreglo debe ser un número positivo.\n");
        return 1;
    }

    pthread_t hilo; // Identificador del hilo 
    int *arreglo = (int*)malloc(size * sizeof(int)); // Reservamos memoria para el arreglo

    // Creamos una instancia de la estructura y la llenamos con nuestros datos
    DatosHilo datos;
    datos.arreglo = arreglo;
    datos.size = size;

    // Creación del hilo
    pthread_create(&hilo, NULL, inicializar_arreglo, &datos);

    // Espera a que el hilo termine para asegurar que el arreglo ya esté inicializado
    pthread_join(hilo, NULL);

    // Mostramos los valores del arreglo de forma secuencial
    printf("Valores del arreglo:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", arreglo[i]);
    }
    printf("\n");
    
    // liberaramos la memoria dinámica asignada
    free(arreglo);

    return 0;
}