# Definir el compilador
CC = gcc

# Definir las banderas de compilación (advertencias y optimización)
CFLAGS = -Wall -g -fopenmp

# Nombres de los archivos ejecutables finales
TARGETS = ej6 ej6_task

# Regla principal por defecto
all: $(TARGETS)

# Regla para construir el ejecutable de la version con parallel for
ej6: ej6.c
	$(CC) $(CFLAGS) ej6.c -o ej6

# Regla para construir el ejecutable de la version con task
ej6_task: ej6_task.c
	$(CC) $(CFLAGS) ej6_task.c -o ej6_task

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGETS)