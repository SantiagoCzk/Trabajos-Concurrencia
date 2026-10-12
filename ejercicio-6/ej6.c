/*
Ejercicio 6

Investigar las directivas single y task de OpenMP. Analizar la generación de números de la 
serie de Fibonacci mediante el código que se presenta abajo. Luego de entenderlo y de 
probarlo en consola, desarrollar una versión usando parallel for en lugar de task, 
tratando de obtener el mismo resultado que la versión presentada. Determinar qué estrategia 
de scheduling debería utilizarse. Por último, comparar los tiempos de ejecución de ambas 
versiones.

Números de Fibonacci con OpenMP:

#include<stdio.h> 
#include<omp.h> 
long fib(int n) 
{ 
  return(n < 2 ? 1 : fib(n - 1) + fib(n - 2)); 
} 
int main(int argc, char* argv[]) 
{ 
  int max; 
    sscanf(argv[1], "%d", &max); 
      #pragma omp parallel 
        #pragma omp single 
          for(int n = 1; n <= max; n++) 
            #pragma omp task 
              printf("%d: %d %ld\n", omp_get_thread_num(), n, fib(n)); 
  return 0;
*/