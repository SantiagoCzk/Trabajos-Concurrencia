/*
    Ejercicio 4

    Cree un programa en C que genere 6 hilos de tipo “h1”, 6 hilos de tipo “h2” y otros 6 hilos
    de tipo “h3”. Todos los hilos se ejecutarán de forma concurrente. Por otro lado se tienen dos
    recursos compartidos, A y B. Los hilos h1 utilizarán el recurso A, los hilos h2 utilizarán el
    recurso B y los hilos h3 utilizarán simultáneamente los recursos A y B. Tanto el recurso A
    como el B deben ser usados en exclusión mutua, esto es, solo un hilo puede acceder de
    forma simultánea al recurso. Por tanto, un hilo “h1” puede acceder al recurso A mientras un
    hilo “h2” está accediendo al recurso B. Sin embargo, cuando un hilo “h3” está accediendo a
    los recursos A y B, los hilos “h1” y “h2” no podrán acceder respectivamente a sus recursos.
    Cuando se habla de “utilizar el recurso”, es simplemente modificar su valor y mostrarlo por
    consola.
    Ejemplo:

    int A; // En algun lugar del programa se define el recurso 
    ...
    A += 10; // Ejemplo de utilizar el recurso
    printf(A);
*/