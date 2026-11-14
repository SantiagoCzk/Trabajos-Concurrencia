#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Función para contar los vecinos encendidos ('1') en la matriz local
int contar_vecinos(int *matriz, int r, int c, int filas, int n) {
    int vecinos = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Omitir la celda central
            
            int nr = r + i;
            int nc = c + j;
            
            if (nr >= 0 && nr < filas && nc >= 0 && nc < n) {
                vecinos += matriz[nr * n + nc];
            }
        }
    }
    return vecinos;
}

int main(int argc, char** argv) {
    // Desactivar buffering de stdout para que los printf salgan al instante en MPI
    setvbuf(stdout, NULL, _IONBF, 0);

    int rank, size;
    
    // Inicialización del entorno MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 5) {
        if (rank == 0) {
            printf("Uso: mpiexec -np <procesos> %s <entrada.txt> <salida.txt> <d> <iteraciones>\n", argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    char *archivo_entrada = argv[1];
    char *archivo_salida = argv[2];
    int d = atoi(argv[3]);
    int iteraciones = atoi(argv[4]);
    
    int num_workers = size - 1;
    if (num_workers <= 0) {
        if (rank == 0) {
            printf("Error: Se requiere al menos 1 coordinador y 1 trabajador (mínimo -np 2).\n");
        }
        MPI_Finalize();
        return 0;
    }

    int n = 0;
    int *imagen = NULL;

    // =========================================================================
    // 1. CARGA DE DATOS (Solo el Coordinador - Rank 0)
    // =========================================================================
    if (rank == 0) {
        FILE *f_in = fopen(archivo_entrada, "r");
        if (!f_in) {
            printf("Error: No se pudo abrir el archivo de entrada '%s'.\n", archivo_entrada);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (fscanf(f_in, "%d", &n) != 1 || n <= 0) {
            printf("Error: Dimensión 'n' inválida en el archivo de entrada.\n");
            fclose(f_in);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (n < num_workers) {
            printf("Error: La dimensión N (%d) debe ser mayor o igual a la cantidad de trabajadores (%d).\n", n, num_workers);
            fclose(f_in);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        imagen = (int*)malloc((size_t)n * (size_t)n * sizeof(int));
        if (!imagen) {
            printf("Error: Memoria insuficiente para matriz %dx%d.\n", n, n);
            fclose(f_in);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < n * n; i++) {
            if (fscanf(f_in, "%d", &imagen[i]) != 1) {
                printf("Error al leer el elemento %d de la matriz en '%s'.\n", i, archivo_entrada);
                fclose(f_in);
                free(imagen);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        fclose(f_in);
        printf("[Coordinador] Matriz %dx%d cargada correctamente desde '%s'.\n", n, n, archivo_entrada);
    }

    // Transmitir la dimensión 'n' a todos los trabajadores mediante Broadcast[cite: 1]
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int filas_por_worker = n / num_workers;

    // =========================================================================
    // 2. LÓGICA DEL PROCESO COORDINADOR (Rank 0)
    // =========================================================================
    if (rank == 0) {
        double t_inicio = MPI_Wtime();

        for (int iter = 0; iter < iteraciones; iter++) {
            // Fase de Distribución: Enviar submatrices + halos a cada worker
            for (int w = 1; w <= num_workers; w++) {
                int fila_inicio = (w - 1) * filas_por_worker;
                int cantidad_filas = (w == num_workers) ? (n - fila_inicio) : filas_por_worker;

                int inicio_envio = (fila_inicio > 0) ? (fila_inicio - 1) : 0;
                int fin_envio = (fila_inicio + cantidad_filas < n) ? (fila_inicio + cantidad_filas) : (n - 1);
                int total_filas_enviar = fin_envio - inicio_envio + 1;

                MPI_Send(&total_filas_enviar, 1, MPI_INT, w, 0, MPI_COMM_WORLD);
                MPI_Send(&imagen[inicio_envio * n], total_filas_enviar * n, MPI_INT, w, 1, MPI_COMM_WORLD);
            }

            // Fase de Recolección: Recibir los bloques ya suavizados
            for (int w = 1; w <= num_workers; w++) {
                int fila_inicio = (w - 1) * filas_por_worker;
                int cantidad_filas = (w == num_workers) ? (n - fila_inicio) : filas_por_worker;

                MPI_Recv(&imagen[fila_inicio * n], cantidad_filas * n, MPI_INT, w, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        double t_fin = MPI_Wtime();

        // =====================================================================
        // 3. GUARDADO DE LA IMAGEN RESULTANTE EN TXT (Rank 0)
        // =====================================================================
        FILE *f_out = fopen(archivo_salida, "w");
        if (!f_out) {
            printf("Error: No se pudo crear el archivo de salida '%s'.\n", archivo_salida);
        } else {
            fprintf(f_out, "%d\n", n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    fprintf(f_out, "%d ", imagen[i * n + j]);
                }
                fprintf(f_out, "\n");
            }
            fclose(f_out);
            printf("[Coordinador] Procesamiento finalizado en %.6f s. Resultado guardado en '%s'.\n", t_fin - t_inicio, archivo_salida);
        }

        free(imagen);

    } else {
        // =====================================================================
        // 4. LÓGICA DE LOS PROCESOS TRABAJADORES (Rank > 0)
        // =====================================================================
        int mi_fila_inicio = (rank - 1) * filas_por_worker;
        int mis_filas_reales = (rank == num_workers) ? (n - mi_fila_inicio) : filas_por_worker;

        for (int iter = 0; iter < iteraciones; iter++) {
            int total_filas_recibidas = 0;

            MPI_Recv(&total_filas_recibidas, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int *sub_imagen = (int*)malloc((size_t)total_filas_recibidas * (size_t)n * sizeof(int));
            int *resultado_local = (int*)malloc((size_t)mis_filas_reales * (size_t)n * sizeof(int));

            MPI_Recv(sub_imagen, total_filas_recibidas * n, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int offset_local = (mi_fila_inicio > 0) ? 1 : 0;

            for (int r = 0; r < mis_filas_reales; r++) {
                for (int c = 0; c < n; c++) {
                    int idx_real = (offset_local + r) * n + c;

                    if (sub_imagen[idx_real] == 1) {
                        int vecinos = contar_vecinos(sub_imagen, offset_local + r, c, total_filas_recibidas, n);
                        resultado_local[r * n + c] = (vecinos < d) ? 0 : 1;
                    } else {
                        resultado_local[r * n + c] = 0;
                    }
                }
            }

            MPI_Send(resultado_local, mis_filas_reales * n, MPI_INT, 0, 2, MPI_COMM_WORLD);

            free(sub_imagen);
            free(resultado_local);
        }
    }

    MPI_Finalize();
    return 0;
}