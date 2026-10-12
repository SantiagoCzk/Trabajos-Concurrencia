#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

long fib(int n) {
    return (n < 2 ? 1 : fib(n - 1) + fib(n - 2));
}

int main(int argc, char* argv[]) {
    int max;
    sscanf(argv[1], "%d", &max);

    double t0 = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        for (int n = 1; n <= max; n++) {
            #pragma omp task
            printf("%d: %d %ld\n", omp_get_thread_num(), n, fib(n));
        }
    }

    double t1 = omp_get_wtime();
    printf("Tiempo task: %f s\n", t1 - t0);
    return 0;
}
