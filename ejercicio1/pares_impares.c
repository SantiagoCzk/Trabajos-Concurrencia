/*
 * Ejercicio 1
 *
 * Cree un programa con 20 procesos, donde cada proceso par (su rank es par) le envíe un 
 * mensaje a su proceso impar consecutivo. Usar una cadena de texto como mensaje. Cada 
 * proceso impar pasará a mayúsculas el mensaje recibido y enviará la respuesta al mismo 
 * rank que le envió el mensaje original. 
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NPROCS        20    /* numero de procesos */
#define MAX_MSG       256   /* tamaño máximo de cada mensaje */
#define TAG_MENSAJE   100   /* par  -> impar */
#define TAG_RESPUESTA 200   /* impar -> par */

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

    /* 2. Verificar que se lanzaron exactamente 20 procesos. */
    if (size != NPROCS) {
        if (rank == 0) {
            fprintf(stderr,
                    "Se requieren %d procesos (se lanzaron %d).\n"
                    "Usa: mpirun -np %d ./pares_impares\n",
                    NPROCS, size, NPROCS);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    /* 3. Texto base: argumentos de la línea de comandos o uno por defecto. */
    char base[MAX_MSG] = "Hola desde el proceso par";

    if (argc > 1) {
        base[0] = '\0';
        for (int i = 1; i < argc; i++) {
            size_t libre = sizeof(base) - strlen(base);
            snprintf(base + strlen(base), libre, "%s%s",
                     (i > 1) ? " " : "", argv[i]);
        }
    }

    /* Cada par agrega su rank para que el mensaje sea único. */
    char mensaje[MAX_MSG];
    snprintf(mensaje, sizeof(mensaje), "%s %d", base, rank);

    if (rank % 2 == 0) {
        /* 4. PROCESO PAR: envía al impar siguiente y espera su respuesta. */
        int impar = rank + 1;
        char respuesta[MAX_MSG];

        check(MPI_Send(mensaje, (int)strlen(mensaje) + 1, MPI_CHAR,
                       impar, TAG_MENSAJE, MPI_COMM_WORLD),
              "MPI_Send");

        printf("[Par %2d] -> envía a %2d : \"%s\"\n", rank, impar, mensaje);
        fflush(stdout);

        check(MPI_Recv(respuesta, MAX_MSG, MPI_CHAR,
                       impar, TAG_RESPUESTA, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
              "MPI_Recv");

        printf("[Par %2d] <- recibe de %2d : \"%s\"\n", rank, impar, respuesta);
        fflush(stdout);

    } else {
        /* 5. PROCESO IMPAR: recibe, pasa a mayúsculas y responde. */
        int par = rank - 1;
        char recibido[MAX_MSG];

        check(MPI_Recv(recibido, MAX_MSG, MPI_CHAR,
                       par, TAG_MENSAJE, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
              "MPI_Recv");

        printf("[Impar %2d] <- recibe de %2d : \"%s\"\n", rank, par, recibido);
        fflush(stdout);

        for (int i = 0; recibido[i] != '\0'; i++)
            recibido[i] = (char)toupper((unsigned char)recibido[i]);

        check(MPI_Send(recibido, (int)strlen(recibido) + 1, MPI_CHAR,
                       par, TAG_RESPUESTA, MPI_COMM_WORLD),
              "MPI_Send");

        printf("[Impar %2d] -> responde a %2d : \"%s\"\n", rank, par, recibido);
        fflush(stdout);
    }

    /* 6. Esperar a todos y finalizar MPI. */
    check(MPI_Barrier(MPI_COMM_WORLD), "MPI_Barrier");

    if (rank == 0) {
        printf("\nLas %d conversaciones finalizaron correctamente.\n", NPROCS / 2);
        fflush(stdout);
    }

    check(MPI_Finalize(), "MPI_Finalize");
    return EXIT_SUCCESS;
}
