# Definir el compilador
CC = gcc

# Definir las banderas de compilación (advertencias y optimización)
CFLAGS = -Wall -g -pthread

# Nombre del archivo ejecutable final
TARGET = ej4

# Regla principal por defecto
all: $(TARGET)

# Regla para construir el ejecutable a partir del archivo C
$(TARGET): ej4.c
	$(CC) $(CFLAGS) ej4.c -o $(TARGET)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)
