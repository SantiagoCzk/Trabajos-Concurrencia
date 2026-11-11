# Compilador MPI estándar (funciona tanto para MPICH como para OpenMPI)[cite: 5, 6]
CC = mpicc

# Banderas de compilación (activan advertencias estándar)
CFLAGS = -Wall -Wextra -std=c99

# Nombre del ejecutable final
TARGET = smoothing_coordinador

# Archivo fuente en C
SRC = smoothing_coordinador.c

# Regla por defecto al ejecutar 'make'
all: $(TARGET)

# Compilación del ejecutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Regla para limpiar archivos generados
clean:
	rm -f $(TARGET) resultado.txt

# Regla falsa para evitar conflictos si existen archivos llamados igual que las reglas
.PHONY: all clean