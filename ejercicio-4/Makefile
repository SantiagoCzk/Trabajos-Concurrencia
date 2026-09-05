# Nombre del archivo ejecutable final
TARGET = ej4

# Archivo fuente en C
SRC = ej4.c

# Compilador y banderas de optimización/advertencia
CC = gcc
CFLAGS = -Wall -Wextra

# Regla principal por defecto
all: $(TARGET)

# Cómo construir el ejecutable a partir del archivo fuente
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Regla para limpiar archivos generados
clean:
	rm -f $(TARGET)
