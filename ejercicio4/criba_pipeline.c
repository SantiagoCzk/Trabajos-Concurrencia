/*
 * criba_pipeline.c - Figura 7.15 del libro de Andrews (CSP) traducida a MPI.
 *
 * Pipeline de procesos:
 *   rank 0        genera el 2 y los impares 3,5,...,N
 *   ranks 1..P-1  cada uno recibe un primo y filtra sus múltiplos
 *   último rank   imprime todo lo que recibe (ya son primos)
 *
 * Uso: mpirun -np 13 ./criba_pipeline [N]
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG_DATO 0

/* Comprueba si con "procesos" etapas el último filtro deja pasar solo primos.
 * Devuelve 1 si es correcto; si no, deja en *minimos el mínimo de procesos. */
static int etapas_suficientes(int n, int procesos, int *minimos)
{
    char *compuesto = calloc((size_t)n + 1, 1);
    int *primos = malloc(sizeof(int) * ((size_t)n + 1));
    int total = 0;

    for (int i = 2; i <= n; i++) {
        if (!compuesto[i]) {
            primos[total++] = i;
            for (long long j = (long long)i * i; j <= n; j += i)
                compuesto[j] = 1;
        }
    }

    int filtros = procesos - 1;
    int valido = 1;

    /* Con más filtros que primos, las etapas sobrantes quedan vacías. */
    if (filtros <= total) {
        long long p = primos[filtros - 1];
        if (p * p < n)
            valido = 0;
    }

    if (!valido) {
        int k = 1;
        while (k <= total && (long long)primos[k - 1] * primos[k - 1] < n)
            k++;
        *minimos = k + 1;
    }

    free(compuesto);
    free(primos);
    return valido;
}

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = (argc > 1) ? atoi(argv[1]) : 1000;
    if (n < 2) {
        if (rank == 0)
            fprintf(stderr, "N debe ser mayor o igual que 2.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    if (size < 2) {
        if (rank == 0)
            fprintf(stderr, "Se requieren al menos 2 procesos.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* Avisar si faltan etapas para garantizar que el último rank solo vea primos. */
    int valido = 1, minimos = 2;
    if (rank == 0)
        valido = etapas_suficientes(n, size, &minimos);
    MPI_Bcast(&valido, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&minimos, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (!valido) {
        if (rank == 0)
            fprintf(stderr,
                    "Con N=%d se necesitan al menos %d procesos "
                    "(se lanzaron %d).\n", n, minimos, size);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0) {
        /* Generador: envía 2 y después los impares, y termina con -1. */
        int valor = 2;
        MPI_Send(&valor, 1, MPI_INT, 1, TAG_DATO, MPI_COMM_WORLD);

        for (valor = 3; valor <= n; valor += 2)
            MPI_Send(&valor, 1, MPI_INT, 1, TAG_DATO, MPI_COMM_WORLD);

        valor = -1;
        MPI_Send(&valor, 1, MPI_INT, 1, TAG_DATO, MPI_COMM_WORLD);
    } else {
        /* Filtro: el primer valor recibido es su primo; luego reenvía los
         * candidatos que no son múltiplos suyos. El último rank imprime. */
        int primo = 0, tengo_primo = 0, valor;

        for (;;) {
            MPI_Recv(&valor, 1, MPI_INT, rank - 1, TAG_DATO,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (valor < 0) {
                if (rank < size - 1)
                    MPI_Send(&valor, 1, MPI_INT, rank + 1, TAG_DATO, MPI_COMM_WORLD);
                break;
            }

            if (!tengo_primo) {
                primo = valor;
                tengo_primo = 1;
                printf("[Etapa %2d] primo %d\n", rank, valor);
                fflush(stdout);
            } else if (rank == size - 1) {
                printf("[Etapa %2d] primo %d\n", rank, valor);
                fflush(stdout);
            } else if (valor % primo != 0) {
                MPI_Send(&valor, 1, MPI_INT, rank + 1, TAG_DATO, MPI_COMM_WORLD);
            }
        }
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
