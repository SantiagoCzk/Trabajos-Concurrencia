Aquí tienes la implementación completa en C para la **Solución A (Centralizada / Coordinador-Trabajadores)** en MPI, junto con la exportación de la imagen resultante a un archivo `.txt` y las instrucciones detalladas para su ejecución distribuida entre múltiples PCs de la red.

---

### 1. Código Fuente Completo (`suavizado_coordinador.c`)

```c
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
    int rank, size;

    // Inicialización del entorno MPI[cite: 1]
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Identificador del proceso[cite: 1]
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Cantidad total de procesos[cite: 1]

    if (argc != 5) {
        if (rank == 0) {
            printf("Uso: mpiexec -np <procesos> %s <entrada.txt> <salida.txt> <d> <iteraciones>\n", argv[0]);
        }
        MPI_Finalize(); // Termina el entorno MPI[cite: 1]
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

        imagen = (int*)malloc(n * n * sizeof(int));
        for (int i = 0; i < n * n; i++) {
            if (fscanf(f_in, "%d", &imagen[i]) != 1) {
                printf("Error al leer los valores de la matriz en el archivo.\n");
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

    int filas_por_worker = n / num_workers; // Cantidad base de filas por trabajador

    // =========================================================================
    // 2. LÓGICA DEL PROCESO COORDINADOR (Rank 0)
    // =========================================================================
    if (rank == 0) {
        for (int iter = 0; iter < iteraciones; iter++) {
            // Fase de Distribución: Enviar submatrices + halos (celdas fantasma) a cada worker
            for (int w = 1; w <= num_workers; w++) {
                int fila_inicio = (w - 1) * filas_por_worker;
                int cantidad_filas = (w == num_workers) ? (n - fila_inicio) : filas_por_worker;

                // Determinar bordes extras (halos superior e inferior)
                int inicio_envio = (fila_inicio > 0) ? (fila_inicio - 1) : 0;
                int fin_envio = (fila_inicio + cantidad_filas < n) ? (fila_inicio + cantidad_filas) : (n - 1);
                int total_filas_enviar = fin_envio - inicio_envio + 1;

                // Enviar cabecera (número de filas que recibe este trabajador)[cite: 1]
                MPI_Send(&total_filas_enviar, 1, MPI_INT, w, 0, MPI_COMM_WORLD);

                // Enviar submatriz con celdas fantasma incorporadas[cite: 1]
                MPI_Send(&imagen[inicio_envio * n], total_filas_enviar * n, MPI_INT, w, 1, MPI_COMM_WORLD);
            }

            // Fase de Recolección: Recibir los bloques ya suavizados
            for (int w = 1; w <= num_workers; w++) {
                int fila_inicio = (w - 1) * filas_por_worker;
                int cantidad_filas = (w == num_workers) ? (n - fila_inicio) : filas_por_worker;

                // Recibir y reconstruir las filas calculadas directamente en la matriz global[cite: 1]
                MPI_Recv(&imagen[fila_inicio * n], cantidad_filas * n, MPI_INT, w, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        // =========================================================================
        // 3. GUARDADO DE LA IMAGEN RESULTANTE EN TXT (Rank 0)
        // =========================================================================
        FILE *f_out = fopen(archivo_salida, "w");
        if (!f_out) {
            printf("Error: No se pudo crear el archivo de salida '%s'.\n", archivo_salida);
        } else {
            fprintf(f_out, "%d\n", n); // Primera línea: dimensión N
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    fprintf(f_out, "%d ", imagen[i * n + j]);
                }
                fprintf(f_out, "\n");
            }
            fclose(f_out);
            printf("[Coordinador] Procesamiento finalizado. Imagen resultante guardada en '%s'.\n", archivo_salida);
        }

        free(imagen);

    } else {
        // =========================================================================
        // 4. LÓGICA DE LOS PROCESOS TRABAJADORES (Rank > 0)
        // =========================================================================
        int mi_fila_inicio = (rank - 1) * filas_por_worker;
        int mis_filas_reales = (rank == num_workers) ? (n - mi_fila_inicio) : filas_por_worker;

        for (int iter = 0; iter < iteraciones; iter++) {
            int total_filas_recibidas;

            // Recibir la cantidad de filas enviadas en este paquete[cite: 1]
            MPI_Recv(&total_filas_recibidas, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int *sub_imagen = (int*)malloc(total_filas_recibidas * n * sizeof(int));
            int *resultado_local = (int*)malloc(mis_filas_reales * n * sizeof(int));

            // Recibir la porción de submatriz con celdas fantasma[cite: 1]
            MPI_Recv(sub_imagen, total_filas_recibidas * n, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Determinar si la primera fila recibida corresponde al halo o a la submatriz real
            int offset_local = (mi_fila_inicio > 0) ? 1 : 0;

            // Cómputo del suavizado sobre el bloque asignado
            for (int r = 0; r < mis_filas_reales; r++) {
                for (int c = 0; c < n; c++) {
                    int idx_real = (offset_local + r) * n + c;

                    if (sub_imagen[idx_real] == 1) {
                        int vecinos = contar_vecinos(sub_imagen, offset_local + r, c, total_filas_recibidas, n);
                        // Aplicación de la regla del umbral d
                        resultado_local[r * n + c] = (vecinos < d) ? 0 : 1;
                    } else {
                        resultado_local[r * n + c] = 0;
                    }
                }
            }

            // Enviar únicamente las filas reales procesadas de regreso al coordinador[cite: 1]
            MPI_Send(resultado_local, mis_filas_reales * n, MPI_INT, 0, 2, MPI_COMM_WORLD);

            free(sub_imagen);
            free(resultado_local);
        }
    }

    // Libera los recursos de la librería MPI[cite: 1]
    MPI_Finalize();
    return 0;
}

```

---

### 2. Configuración y Ejecución Distribuida (Múltiples PCs)

Para que este programa corra en un clúster o red de computadoras físicas interconectadas mediante pasaje de mensajes en MPI, se deben realizar los siguientes pasos de infraestructura y despliegue:

#### A. Requisitos Previos en las PCs de la Red

1. **Red Local:** Todas las computadoras deben estar conectadas a la misma red física (LAN) y tener direcciones IP fijas o alcanzables (ej: `192.168.0.10`, `192.168.0.11`).
2. **Acceso por SSH sin contraseña:** El nodo principal (Coordinador) debe poder ingresar a los nodos trabajadores vía SSH sin solicitar clave. Esto se logra ejecutando en la PC principal:

```bash
ssh-keygen -t rsa
ssh-copy-id usuario@192.168.0.11

```

3. **Misma Ruta de Archivos:** El binario compilado (`suavizado_coordinador`) y el archivo de imagen de entrada (`entrada.txt`) deben existir exactamente en la **misma ruta absoluta** en todas las computadoras (por ejemplo, en `/home/usuario/tp_concurrencia/`), o estar montados sobre un directorio NFS compartido.

#### B. Compilación

Compilar el programa en todas las computadoras con el compilador estándar de MPI (`mpicc`):

```bash
mpicc suavizado_coordinador.c -o suavizado_coordinador

```

#### C. Crear el archivo de lista de computadoras (`machinefile` / `hostfile`)

Crea un archivo llamado `machinefile` en la PC del coordinador indicando las direcciones IP y la asignación de procesos (`slots`):

- Para OpenMPI (`--hostfile machinefile`):

```text
192.168.0.10 slots=1
192.168.0.11 slots=4

```

(La PC `192.168.0.10` ejecutará 1 proceso correspondiente al Coordinador, y la PC `192.168.0.11` ejecutará 4 procesos trabajadores).

- Para MPICH (`-f machinefile`):

```text
192.168.0.10:1
192.168.0.11:4

```

#### D. Comando de Ejecución Distribuida

Para ejecutar la aplicación con 5 procesos en total (1 Coordinador + 4 Trabajadores) distribuidos en la red, filtrando con un umbral $d = 3$ durante $10$ iteraciones:

- Con OpenMPI:

```bash
mpiexec --hostfile machinefile -np 5 ./suavizado_coordinador entrada.txt resultado.txt 3 10

```

- Con MPICH:

```bash
mpiexec -f machinefile -np 5 ./suavizado_coordinador entrada.txt resultado.txt 3 10

```

---

### 3. Ejemplo de Formato del Archivo de Entrada y Salida `.txt`

#### Archivo de Entrada (`entrada.txt`)

```text
4
1 1 0 1
0 1 1 0
1 0 1 1
0 1 0 0

```

#### Archivo Generado de Salida (`resultado.txt`)

El proceso coordinador generará automáticamente la nueva matriz en el disco de la PC principal con la primera línea indicando la dimensión $n$ seguida del contenido procesado:

```text
4
0 1 0 0
0 1 1 0
0 0 0 0
0 0 0 0

```
