# Definir el compilador
CC = gcc

# Definir las banderas de compilación (advertencias y optimización)
CFLAGS = -Wall -g

# Nombre del archivo ejecutable final
TARGET = ej2

# Regla principal por defecto
all: $(TARGET)

# Regla para construir el ejecutable a partir del archivo C
$(TARGET): ej2.c
	$(CC) $(CFLAGS) ej2.c -o $(TARGET)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)
