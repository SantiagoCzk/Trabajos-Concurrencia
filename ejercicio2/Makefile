# Makefile - Ejercicio 2: mensajes del raíz con MPI_ANY_SOURCE / MPI_ANY_TAG
#
#   make         compila
#   make run     ejecuta con 8 procesos
#   make clean   borra el ejecutable
#   make help    muestra la ayuda

CC      := mpicc
CFLAGS  := -Wall -Wextra -O2 -g -std=c11
TARGET  := mensaje_raiz
SRC     := $(TARGET).c
NP      := 8
ARGS    ?=

.DEFAULT_GOAL := all
.PHONY: all run clean help

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	mpirun -np $(NP) ./$(TARGET) $(ARGS)

clean:
	rm -f $(TARGET)

help:
	@echo "make                    -> compila $(TARGET)"
	@echo "make run                -> ejecuta con $(NP) procesos"
	@echo "make run NP=10          -> ejecuta con otra cantidad de procesos"
	@echo "make run ARGS=\"texto\"   -> ejecuta con un texto base"
	@echo "make clean              -> elimina el ejecutable"
