/*
    Ejercicio 5

    Resolver el problema del puente de una sola vía con semáforos. Los autos que vienen del
    norte y del sur se encuentran con un puente de una sola vía. Los autos que van en el mismo
    sentido pueden atravesar el puente en forma simultánea. En cambio, si se encuentran dos
    autos en sentidos opuestos, sólo uno puede atravesar el puente, mientras el otro debe esperar.
    En la espera se puede formar una fila de autos. Mientras hayan autos cruzando en un
    sentidos, pueden seguir entrando autos al puente en el mismo sentido.
    Desarrollar un programa que reciba la cantidad de autos como parámetro y cree esta
    cantidad de autos a cada lado del puente. Cada auto estará representado por un hilo que pasa
    un tiempo aleatorio en el lado creado y luego intenta cruzar el puente. Una vez que sale del
    otro lado termina su ejecución. La espera aleatoria será de hasta 30 segundos. El cruce del
    puente demanda un tiempo fijo de 400ms.
    Utilizar impresiones a pantalla que muestren claramente toda la simulación.
*/