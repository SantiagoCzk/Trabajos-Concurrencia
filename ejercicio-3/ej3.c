/*
Ejercicio 3

Cree un programa en C que realice dos funciones de manera concurrente. Estas funciones se 
llamarán “funcion_tiempo_1” y “funcion_tiempo_2”. Cada función hará un sleep(1) y 
sleep(2) respectivamente. Para esto debe usar la directiva “sections”. Medir el tiempo de 
ejecución de todo el programa con la función “omp_get_wtime()”, y mostrarlo por consola. 
Cada función genera un double, que al restarlos, se obtiene el tiempo de duración. Ejemplo:

double inicio, fin;
inicio = omp_get_wtime();
... Procesamiento ...
fin = omp_get_wtime();
resultado = fin – inicio;
*/