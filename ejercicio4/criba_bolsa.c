/*
 * criba_bolsa.c - Figura 7.16 del libro de Andrews (Linda) adaptada a MPI.
 *
 * Bolsa de tareas:
 *   rank 0        gestor: reparte candidatos y guarda los resultados
 *   ranks 1..P-1  workers: piden trabajo y comprueban divisibilidad
 *
 * Cada worker calcula al inicio los primos hasta sqrt(N), así que el gestor
 * solo se ocupa de repartir candidatos y recoger resultados.
 *
 * Uso: mpirun -np 8 ./criba_bolsa [N]
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAG_TRABAJO   10
#define TAG_RESULTADO 20
#define FIN_TAREAS    (-1)

/* Llena "primos" con los primos <= limite y devuelve cuántos son. */
static int calcular_primos(int limite, int *primos)
{
    int total = 0;

    for (int v = 2; v <= limite; v++) {
        int es_primo = 1;
        for (int i = 0; i < total && primos[i] * primos[i] <= v; i++) {
            if (v % primos[i] == 0) {
                es_primo = 0;
                break;
            }
        }
        if (es_primo)
            primos[total++] = v;
    }
    return total;
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

    if (rank == 0) {
        /* GESTOR: bolsa de candidatos 5,7,...,N y tabla de resultados. */
        char *es_primo = calloc((size_t)n + 1, 1);
        if (n >= 2) es_primo[2] = 1;
        if (n >= 3) es_primo[3] = 1;

        int total_candidatos = (n >= 5) ? ((n - 5) / 2 + 1) : 0;
        int siguiente = 5;
        int recibidos = 0, stops = 0;
        int workers = size - 1;

        while (recibidos < total_candidatos || stops < workers) {
            int msg[2];
            MPI_Status estado;

            MPI_Recv(msg, 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &estado);

            if (estado.MPI_TAG == TAG_TRABAJO) {
                /* Un worker pide trabajo: dar candidato o despedirlo. */
                msg[0] = (siguiente <= n) ? siguiente : FIN_TAREAS;
                msg[1] = 0;
                MPI_Send(msg, 2, MPI_INT, estado.MPI_SOURCE,
                         TAG_TRABAJO, MPI_COMM_WORLD);

                if (siguiente <= n)
                    siguiente += 2;
                else
                    stops++;
            } else {
                /* Resultado: msg[0] = candidato, msg[1] = ¿es primo? */
                es_primo[msg[0]] = (char)msg[1];
                recibidos++;
            }
        }

        printf("[Gestor] primos hasta %d:\n", n);
        for (int v = 2; v <= n; v++)
            if (es_primo[v])
                printf("%d\n", v);
        fflush(stdout);

        free(es_primo);
    } else {
        /* WORKER: tabla local de primos hasta sqrt(N). */
        int limite = 0;
        while ((limite + 1) * (limite + 1) <= n)
            limite++;

        int *primos = malloc(sizeof(int) * (size_t)(limite + 1));
        int total = calcular_primos(limite, primos);

        for (;;) {
            int msg[2] = {0, 0};

            /* Pedir trabajo al gestor. */
            MPI_Send(msg, 2, MPI_INT, 0, TAG_TRABAJO, MPI_COMM_WORLD);
            MPI_Recv(msg, 2, MPI_INT, 0, TAG_TRABAJO,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (msg[0] == FIN_TAREAS)
                break;

            /* Probar el candidato por división entre los primos conocidos. */
            int candidato = msg[0];
            int es_primo = 1;
            for (int i = 0; i < total; i++) {
                if (primos[i] * primos[i] > candidato)
                    break;
                if (candidato % primos[i] == 0) {
                    es_primo = 0;
                    break;
                }
            }

            msg[0] = candidato;
            msg[1] = es_primo;
            MPI_Send(msg, 2, MPI_INT, 0, TAG_RESULTADO, MPI_COMM_WORLD);
        }

        free(primos);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
