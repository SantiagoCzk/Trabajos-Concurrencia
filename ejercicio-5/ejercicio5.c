/*
	Ejercicio 5

	Cree la versión 3 del programa mm.c (multiplicación de matrices), basado en la versión 
	2 disponible en el Aula, pero mejorado. Esta versión debe repartir el trabajo en H hilos. 
	H debería dividir exactamente a N, siendo N el tamaño de la matriz. El valor H se pasará 
	como parámetro en la invocación del programa. Utilice un tamaño de matriz de 
	1024x1024 o mayor, si la memoria disponible lo permite.
*/

/* Esta versión 3 recibe H (cantidad de hilos) como parámetro por línea
 * de comandos. H debe dividir exactamente a N. Cada hilo procesa un
 * bloque contiguo de N/H filas. */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#define N 1024

/* Matrices globales: van en el segmento de datos del programa, no en el
 * stack. Por eso el problema de N=1024 NO tiene que ver con el stack de
 * los hilos, sino con el tamaño de estos arrays estáticos. */
double a[N][N];
double b[N][N];
double c[N][N];

/* Argumento que recibe cada hilo: el rango de filas [start_row, end_row)
 * que le toca procesar. Se pasa por puntero (malloc) en vez de castear
 * un int a void*, que es lo que hacía la v2 y no es portable a 64 bits.
 * Es "medio abierto": incluye start_row pero NO incluye end_row (por eso
 * el for de worker usa "i < end_row"). */
typedef struct {
	int start_row;
	int end_row;
} thread_arg_t;

void iniMat(double mat[][N], int len, double iniVal)
{
	int i, j;
	for (i = 0; i < len; ++i)
		for (j = 0; j < len; ++j)
			mat[i][j] = iniVal;
}

/* Check1Mat: Chequea que la matriz resultante quede con el mismo valor en todas sus celdas.
 * Retorna: 0 si está correcta, 1 si hay algún valor distinto de finVal. */
int Check1Mat(double mat[][N], int len, double finVal)
{
	int i, j;
	for (i = 0; i < len; ++i)
		for (j = 0; j < len; ++j)
			if (mat[i][j] != finVal)
				return 1;
	return 0;
}

void printMat(double mat[][N], int len)
{
	int i, j;
	for (i = 0; i < len; ++i) {
		for (j = 0; j < len; ++j)
			printf("%.1f ", mat[i][j]);
		printf("\n");
	}
}

/* worker: función que ejecuta CADA hilo.
 *
 * Punto clave del ejercicio: en vez de calcular una sola fila (como la v2),
 * cada hilo recorre TODO su bloque de filas [start_row, end_row).
 *
 * No hay condición de carrera: cada hilo escribe únicamente en las filas
 * de c que le corresponden a él (c[i][j] con i dentro de su propio rango).
 * Ningún otro hilo toca esas mismas filas, así que no hace falta mutex
 * ni ningún tipo de sincronización durante el cálculo en sí. */
void *worker(void *arg)
{
	/* Casteamos el void* de vuelta a nuestra struct para recuperar
	 * el rango de filas que le tocó a este hilo. */
	thread_arg_t *targ = (thread_arg_t *) arg;
	int i, j, k;

	for (i = targ->start_row; i < targ->end_row; ++i)
		for (j = 0; j < N; ++j)
			for (k = 0; k < N; ++k)
				c[i][j] += a[i][k] * b[k][j];

	/* Liberamos la memoria reservada en mm() para este hilo en
	 * particular, ya que cada hilo es responsable de su propio
	 * argumento. Si no se libera, hay fuga de memoria (memory leak). */
	free(targ);
	return NULL;
}

/* mm: crea los H hilos, reparte el trabajo y espera a que todos terminen. */
void mm(int H)
{
	pthread_t *hilos;
	int i, retval, rows_per_thread;

	/* Reparto del trabajo: como ya validamos en main() que H divide
	 * exactamente a N, esta división es siempre exacta (sin resto). */
	rows_per_thread = N / H;

	/* Antes se usaba un arreglo fijo hilos[N]; acá reservamos dinámicamente
	 * solo el espacio necesario para H hilos (H puede ser mucho menor que N). */
	hilos = malloc(H * sizeof(pthread_t));
	if (hilos == NULL) {
		perror("malloc");
		exit(1);
	}

	for (i = 0; i < H; ++i) {
		/* A cada hilo le corresponde un bloque distinto y contiguo
		 * de filas. Ejemplo con N=1024, H=2:
		 *   hilo 0 -> filas [0, 512)
		 *   hilo 1 -> filas [512, 1024)
		 * que es exactamente el reparto que pide la consigna. */
		thread_arg_t *targ = malloc(sizeof(thread_arg_t));
		if (targ == NULL) {
			perror("malloc");
			exit(1);
		}
		targ->start_row = i * rows_per_thread;
		targ->end_row = targ->start_row + rows_per_thread;

		/* Se le pasa un puntero a la struct (no un int casteado),
		 * así el hilo sabe exactamente qué rango de filas procesar. */
		retval = pthread_create(&hilos[i], NULL, &worker, (void *) targ);
		if (retval != 0) {
			perror("pthread_create");
			exit(1);
		}
	}

	/* pthread_join: el hilo principal espera acá a que los H hilos
	 * terminen su trabajo antes de seguir. Sin esto, main() podría
	 * chequear el resultado en c[][] antes de que esté completo. */
	for (i = 0; i < H; ++i)
		pthread_join(hilos[i], NULL);

	free(hilos);
}

int main(int argc, char *argv[])
{
	int H;
	struct timespec ini, fin;
	double segundos;

	/* Validamos ejecución válida */
	if (argc != 2) {
		fprintf(stderr, "Uso: %s <cantidad_de_hilos>\n", argv[0]);
		exit(1);
	}

	H = atoi(argv[1]);

	/* Validaciones */
	if (H <= 0) {
		fprintf(stderr, "Error: la cantidad de hilos debe ser un entero positivo.\n");
		exit(1);
	}
	if (H > N) {
		fprintf(stderr, "Error: H (%d) no puede ser mayor que N (%d).\n", H, N);
		exit(1);
	}
	if (N % H != 0) {
	// H tiene que dividir exactamente a N *
		fprintf(stderr, "Error: H (%d) debe dividir exactamente a N (%d).\n", H, N);
		exit(1);
	}

	printf("Comienzo ... (N=%d, H=%d, filas por hilo=%d)\n", N, H, N / H);
	iniMat(a, N, 1.0);
	iniMat(b, N, 1.0);
	iniMat(c, N, 0.0);

	printf("Multiplicando ...\n");

	/* Medición de tiempo: se toma solo alrededor de mm(H), para que el
	 * tiempo reportado sea el de la multiplicación en sí, sin contar
	 * la inicialización de las matrices (iniMat). Esto es lo que permite
	 * comparar de forma justa distintos valores de H, como pide la
	 * consigna ("Realice pruebas de tiempos con diferentes valores de H"). */
	clock_gettime(CLOCK_MONOTONIC, &ini);
	mm(H);
	clock_gettime(CLOCK_MONOTONIC, &fin);

	segundos = (fin.tv_sec - ini.tv_sec) + (fin.tv_nsec - ini.tv_nsec) / 1e9;

	if (Check1Mat(c, N, N) == 0)
		printf("Fin Multiplicación (Resultado correcto)\n");
	else
		printf("Fin Multiplicación (Resultado INCORRECTO!)\n");

	printf("Tiempo de multiplicación (solo cómputo, sin iniMat): %.4f segundos\n", segundos);

	/*
	printf("Matriz a:\n");
	printMat(a, N);
	printf("Matriz b:\n");
	printMat(b, N);
	printf("Matriz c:\n");
	printMat(c, N);
	*/

	printf("Fin del programa\n");
	return 0;
}