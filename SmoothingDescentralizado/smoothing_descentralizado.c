#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int contar_vecinos(int *matriz, int r, int c, int filas, int n) {
    int vecinos = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; 
            
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
    setvbuf(stdout, NULL, _IONBF, 0);

    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 5) {
        if (rank == 0) {
            printf("Uso: mpiexec -np <procesos> %s <entrada.txt> <salida.txt> <d (1-8)> <iteraciones>\n", argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    char *archivo_entrada = argv[1];
    char *archivo_salida = argv[2];
    int d = atoi(argv[3]);
    int iteraciones = atoi(argv[4]);

    if (d < 1 || d > 8) {
        if (rank == 0) {
            printf("Error: 'd' debe estar entre 1 y 8.\n");
        }
        MPI_Finalize();
        return 0;
    }

    int n = 0;
    int *imagen_global = NULL;

    // =========================================================================
    // 1. CARGA INCIAL (Rank 0 lee y avisa la dimensión 'n')
    // =========================================================================
    if (rank == 0) {
        FILE *f_in = fopen(archivo_entrada, "r");
        if (!f_in) {
            printf("Error: No se pudo abrir '%s'.\n", archivo_entrada);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fscanf(f_in, "%d", &n);
        imagen_global = (int*)malloc((size_t)n * n * sizeof(int));
        
        for (int i = 0; i < n * n; i++) {
            fscanf(f_in, "%d", &imagen_global[i]);
        }
        fclose(f_in);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cálculo de filas que le tocan a CADA proceso (ahora el rank 0 también trabaja)
    int filas_base = n / size;
    int mis_filas = (rank == size - 1) ? (n - rank * filas_base) : filas_base;

    // Buffer local: incluye 1 fila extra arriba (halo) y 1 extra abajo (halo)
    int *buffer_local = (int*)calloc((mis_filas + 2) * n, sizeof(int));
    int *resultado_local = (int*)calloc((mis_filas + 2) * n, sizeof(int));

    // =========================================================================
    // 2. REPARTO INICIAL (Rank 0 reparte, todos reciben)
    // =========================================================================
    if (rank == 0) {
        // Copia su propia parte al centro de su buffer (fila 1 a mis_filas)
        for(int i = 0; i < mis_filas * n; i++) {
            buffer_local[n + i] = imagen_global[i];
        }
        // Envía el resto a los demás
        for (int p = 1; p < size; p++) {
            int filas_p = (p == size - 1) ? (n - p * filas_base) : filas_base;
            int inicio = p * filas_base;
            MPI_Send(&imagen_global[inicio * n], filas_p * n, MPI_INT, p, 0, MPI_COMM_WORLD);
        }
    } else {
        // Reciben su bloque y lo guardan a partir de la fila 1 (dejando la 0 para el halo superior)
        MPI_Recv(&buffer_local[n], mis_filas * n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // =========================================================================
    // 3. CICLO DE SUAVIZADO DESCENTRALIZADO (Peer-to-Peer)
    // =========================================================================
    double t_inicio = MPI_Wtime();

    int vecino_arriba = (rank > 0) ? rank - 1 : MPI_PROC_NULL;
    int vecino_abajo = (rank < size - 1) ? rank + 1 : MPI_PROC_NULL;

    for (int iter = 0; iter < iteraciones; iter++) {
        
        // Intercambiar bordes usando Sendrecv para evitar deadlocks
        // Envía mi fila 1 al de arriba, recibo en mi fila 0 (halo superior)
        MPI_Sendrecv(&buffer_local[1 * n], n, MPI_INT, vecino_arriba, 0,
                     &buffer_local[0 * n], n, MPI_INT, vecino_arriba, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Envía mi última fila al de abajo, recibo en la fila final (halo inferior)
        MPI_Sendrecv(&buffer_local[mis_filas * n], n, MPI_INT, vecino_abajo, 1,
                     &buffer_local[(mis_filas + 1) * n], n, MPI_INT, vecino_abajo, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Procesar solo las filas reales locales (de 1 a mis_filas)
        for (int r = 1; r <= mis_filas; r++) {
            for (int c = 0; c < n; c++) {
                if (buffer_local[r * n + c] == 1) {
                    int vecinos = contar_vecinos(buffer_local, r, c, mis_filas + 2, n);
                    resultado_local[r * n + c] = (vecinos < d) ? 0 : 1;
                } else {
                    resultado_local[r * n + c] = 0;
                }
            }
        }

        // Sobrescribir el buffer local con los nuevos resultados para la próxima iteración
        for(int i = 1; i <= mis_filas; i++) {
            for(int j = 0; j < n; j++) {
                buffer_local[i * n + j] = resultado_local[i * n + j];
            }
        }
    }

    double t_fin = MPI_Wtime();

    // =========================================================================
    // 4. RECOLECCIÓN Y GUARDADO (Rank 0 junta todo)
    // =========================================================================
    if (rank == 0) {
        // Copia sus resultados definitivos a la matriz global
        for(int i = 0; i < mis_filas * n; i++) {
            imagen_global[i] = buffer_local[n + i];
        }
        // Recibe los resultados de los demás
        for (int p = 1; p < size; p++) {
            int filas_p = (p == size - 1) ? (n - p * filas_base) : filas_base;
            int inicio = p * filas_base;
            MPI_Recv(&imagen_global[inicio * n], filas_p * n, MPI_INT, p, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        FILE *f_out = fopen(archivo_salida, "w");
        if (f_out) {
            fprintf(f_out, "%d\n", n);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    fprintf(f_out, "%d ", imagen_global[i * n + j]);
                }
                fprintf(f_out, "\n");
            }
            fclose(f_out);
            printf("[P2P] Matriz %dx%d procesada en %.6f s. Resultado en '%s'.\n", n, n, t_fin - t_inicio, archivo_salida);
        }
        free(imagen_global);
    } else {
        // Los trabajadores envían sus filas reales terminadas al Rank 0
        MPI_Send(&buffer_local[n], mis_filas * n, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    free(buffer_local);
    free(resultado_local);
    MPI_Finalize();
    return 0;
}