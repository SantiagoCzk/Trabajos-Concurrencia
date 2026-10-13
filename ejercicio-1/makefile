# Nombre del ejecutable
TARGET = ej1

# Archivo fuente
SRC = ej1.c

# Compilador
CC = gcc

# Banderas de compilacion (agregamos -fopenmp para OpenMP)
CFLAGS = -Wall -Wextra -fopenmp

# Regla por defecto
all: $(TARGET)

# Compilacion
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Limpieza de ejecutables
clean:
	rm -f $(TARGET)