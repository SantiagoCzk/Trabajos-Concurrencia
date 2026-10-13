# Definir el compilador
CC = gcc

# Definir las banderas de compilación (advertencias y optimización)
CFLAGS = -Wall -g -fopenmp

# Archivo fuente
SRC = ej2.c

# Nombre del archivo ejecutable final
TARGET = ej2

# Regla principal por defecto
all: $(TARGET)

# Regla para construir el ejecutable a partir del archivo C
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)