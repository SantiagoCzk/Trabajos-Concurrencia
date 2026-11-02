# Makefile - Ejercicio 1: comunicación par/impar con MPI
#
#   make         compila
#   make run     ejecuta con 20 procesos
#   make clean   borra el ejecutable
#   make help    muestra la ayuda

CC      := mpicc
CFLAGS  := -Wall -Wextra -O2 -g -std=c11
TARGET  := pares_impares
SRC     := $(TARGET).c
NP      := 20
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
	@echo "make run ARGS=\"texto\"   -> ejecuta con un texto base"
	@echo "make clean              -> elimina el ejecutable"
