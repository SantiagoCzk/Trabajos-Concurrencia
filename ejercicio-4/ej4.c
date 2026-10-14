/*
Ejercicio 4

Dado la siguiente solución secuencial escrita en C, la cual realiza el cálculo de pi por 
aproximación, “convertirla” a concurrente usando OpenMP y la cláusula “reduction”. Al 
probar variando la cantidad de “num_steps” se va a conseguir mayor o menor precisión. A 
continuación se verá la explicación gráfica del cálculo en sí mismo y el código secuencial:

Solución secuencial:
static long num_steps = 100000; //Factor de precisión
double step;
void main ()
{
int i; double x, pi, sum = 0.0;
step = 1.0/(double) num_steps;
for (i=0; i< num_steps; i++) {
x = (i+0.5)*step;
sum = sum + 4.0/(1.0+x*x);
}
pi = step * sum;
}
*/

#include <stdio.h>
#include <omp.h>

static long num_steps = 100000;  // Factor de precisión
double step;

int main(void) {
    double pi, sum = 0.0;

    step = 1.0 / (double) num_steps;

    double inicio = omp_get_wtime();

    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    pi = step * sum;

    double fin = omp_get_wtime();

    printf("pi = %.10f\n", pi);
    printf("num_steps = %ld\n", num_steps);
    printf("Tiempo: %f s\n", fin - inicio);

    return 0;
}