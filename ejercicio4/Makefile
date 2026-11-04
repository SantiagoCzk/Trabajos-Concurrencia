# Makefile - Ejercicio 4: Criba de Eratóstenes en MPI
#
#   make         compila las dos versiones
#   make run1    ejecuta la versión pipeline (figura 7.15)
#   make run2    ejecuta la versión bolsa de tareas (figura 7.16)
#   make run     ejecuta ambas
#   make clean   borra los ejecutables
#   make help    muestra la ayuda

CC      := mpicc
CFLAGS  := -Wall -Wextra -O2 -g -std=c11
PIPE    := criba_pipeline
BOLSA   := criba_bolsa

N       ?= 1000
NP1     ?= 13
NP2     ?= 8

.DEFAULT_GOAL := all
.PHONY: all run run1 run2 clean help

all: $(PIPE) $(BOLSA)

$(PIPE): $(PIPE).c
	$(CC) $(CFLAGS) -o $@ $<

$(BOLSA): $(BOLSA).c
	$(CC) $(CFLAGS) -o $@ $<

run1: $(PIPE)
	mpirun -np $(NP1) ./$(PIPE) $(N)

run2: $(BOLSA)
	mpirun -np $(NP2) ./$(BOLSA) $(N)

run: run1 run2

clean:
	rm -f $(PIPE) $(BOLSA)

help:
	@echo "make                 -> compila $(PIPE) y $(BOLSA)"
	@echo "make run1            -> pipeline con $(NP1) procesos y N=$(N)"
	@echo "make run2            -> bolsa con $(NP2) procesos y N=$(N)"
	@echo "make run             -> ejecuta las dos versiones"
	@echo "make run1 N=100      -> cambia el límite"
	@echo "make clean           -> elimina los ejecutables"
