#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

static long num_steps = 100000;  // Factor de precisión

int main(int argc, char *argv[]) {
    int rank, size;
    double step;
    double local_sum = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    step = 1.0 / (double) num_steps;

    double inicio = 0.0;
    if (rank == 0) {
        inicio = MPI_Wtime();
    }

    // Arreglo para calcular los rangos de iteración [inicio, fin] de cada proceso
    long *ranges = NULL;
    long my_range[2];

    if (rank == 0) {
        ranges = (long *) malloc(2 * size * sizeof(long));
        long base_steps = num_steps / size;
        long remainder = num_steps % size;

        long current_start = 0;
        for (int i = 0; i < size; i++) {
            long current_count = base_steps + (i < remainder ? 1 : 0);
            ranges[2 * i] = current_start;
            ranges[2 * i + 1] = current_start + current_count;
            current_start += current_count;
        }
    }

    // 1. MPI_Scatter: Envía 2 datos (rango inicial y final) a cada proceso
    MPI_Scatter(ranges, 2, MPI_LONG, my_range, 2, MPI_LONG, 0, MPI_COMM_WORLD);

    // Cada proceso realiza el cálculo local en su subintervalo asignado
    for (long i = my_range[0]; i < my_range[1]; i++) {
        double x = (i + 0.5) * step;
        local_sum += 4.0 / (1.0 + x * x);
    }

    // Arreglo donde el proceso 0 recolectará la suma de cada proceso
    double *all_sums = NULL;
    if (rank == 0) {
        all_sums = (double *) malloc(size * sizeof(double));
    }

    // 2. MPI_Gather: Reúne el valor de local_sum de cada hilo en all_sums
    MPI_Gather(&local_sum, 1, MPI_DOUBLE, all_sums, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // El proceso raíz acumula las sumas parciales e imprime el resultado final
    if (rank == 0) {
        double sum = 0.0;
        for (int i = 0; i < size; i++) {
            sum += all_sums[i];
        }

        double pi = step * sum;
        double fin = MPI_Wtime();

        printf("pi = %.10f\n", pi);
        printf("num_steps = %ld\n", num_steps);
        printf("Tiempo: %f s\n", fin - inicio);

        free(ranges);
        free(all_sums);
    }

    MPI_Finalize();
    return 0;
}