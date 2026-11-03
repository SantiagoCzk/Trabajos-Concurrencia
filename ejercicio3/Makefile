# Makefile - Ejercicio 3: Cálculo de PI con MPI_Scatter y MPI_Gather
#
#   make         compila el programa
#   make run     ejecuta con 4 procesos por defecto
#   make clean   borra el ejecutable
#   make help    muestra la ayuda

CC      := mpicc
CFLAGS  := -Wall -Wextra -O2 -g -std=c11
TARGET  := ej3
SRC     := $(TARGET).c
NP      ?= 4
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
	@echo "make                     -> compila $(TARGET)"
	@echo "make run                 -> ejecuta con $(NP) procesos"
	@echo "make run NP=8            -> ejecuta especificando cantidad de procesos"
	@echo "make clean               -> elimina el ejecutable"