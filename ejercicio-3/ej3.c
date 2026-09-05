/*
    Ejercicio 3

    Implemente la solución del productor/consumidor con mutex y variable de condición.
    Utilice un buffer de 100 posiciones. Se debe crear 2 hilo productor y 2 hilos consumidores.
    Recordemos los detalles a tener en cuenta:

    a) Los productores escriben un carácter aleatorio en el buffer si el mismo no está lleno.
    b) Para el caso de los consumidores, tendremos dos funcionamientos distintos:
        i. El “consumidor 1” funcionará de la forma normal; Si el buffer no está vacío, leer un
        carácter del mismo.
        ii. El “consumidor 2” tendrá un leve cambio con respecto al anterior. Solo leerá un
        valor del buffer, si el mismo al menos tiene 10 caracteres sin consumir.
        iii. Cuando los hilos consumidores se encuentran bloqueados en una variable de
        condición, se debe despertar a todos los hilos.
*/