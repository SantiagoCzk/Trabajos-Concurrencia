/*
 * ejercicio 2
 *
 * Cree un programa donde el proceso raíz envíe un mensaje al resto de los procesos sin 
 * importar el tag (ver MPI_ANY_SOURCE y MPI_ANY_TAG). Cada mensaje debe ser 
 * particular para cada proceso (un mensaje distinto para cada uno). El proceso que recibe el 
 * mensaje debe imprimirlo por consola indicando su propio rank y el rank del destinatario.
 *
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MSG 256

/* Aborta si una llamada a MPI devuelve error. */
static void check(int rc, const char *operacion)
{
    if (rc != MPI_SUCCESS) {
        fprintf(stderr, "Error en %s\n", operacion);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int rank, size;

    /* 1. Iniciar MPI y obtener rank y cantidad de procesos. */
    check(MPI_Init(&argc, &argv), "MPI_Init");
    check(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "MPI_Comm_rank");
    check(MPI_Comm_size(MPI_COMM_WORLD, &size), "MPI_Comm_size");

    /* Se necesita al menos el raíz y un receptor. */
    if (size < 2) {
        if (rank == 0)
            fprintf(stderr, "Se requieren al menos 2 procesos.\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* 2. Texto base: argumentos de la línea de comandos o uno por defecto. */
    char base[MAX_MSG] = "Mensaje privado";

    if (argc > 1) {
        base[0] = '\0';
        for (int i = 1; i < argc; i++) {
            size_t libre = sizeof(base) - strlen(base);
            snprintf(base + strlen(base), libre, "%s%s",
                     (i > 1) ? " " : "", argv[i]);
        }
    }

    if (rank == 0) {
        /* 3. RAÍZ: envía un mensaje personalizado a cada proceso. */
        char mensaje[MAX_MSG];

        for (int destino = 1; destino < size; destino++) {
            snprintf(mensaje, sizeof(mensaje), "%s para el proceso %d",
                     base, destino);

            /* Un tag distinto por destino: el receptor no lo necesita
             * conocer de antemano porque usa MPI_ANY_TAG. */
            int tag = 1000 + destino;

            check(MPI_Send(mensaje, (int)strlen(mensaje) + 1, MPI_CHAR,
                           destino, tag, MPI_COMM_WORLD),
                  "MPI_Send");

            printf("[Raíz %d] -> proceso %d (tag %d) : \"%s\"\n",
                   rank, destino, tag, mensaje);
            fflush(stdout);
        }
    } else {
        /* 4. RECEPTOR: acepta cualquier emisor y cualquier tag. */
        char recibido[MAX_MSG];
        MPI_Status estado;

        check(MPI_Recv(recibido, MAX_MSG, MPI_CHAR,
                       MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &estado),
              "MPI_Recv");

        /* MPI_Status guarda quién envió y con qué tag llegó realmente. */
        printf("[Rank %d] recibió de rank %d : \"%s\" (tag %d)\n",
               rank, estado.MPI_SOURCE, recibido, estado.MPI_TAG);
        fflush(stdout);
    }

    /* 5. Sincronizar y finalizar. */
    check(MPI_Barrier(MPI_COMM_WORLD), "MPI_Barrier");

    if (rank == 0) {
        printf("\nTodos los procesos recibieron su mensaje.\n");
        fflush(stdout);
    }

    check(MPI_Finalize(), "MPI_Finalize");
    return EXIT_SUCCESS;
}
