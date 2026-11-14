import argparse
import random


def generar_pino(n):
    matriz = [[0] * n for _ in range(n)]
    centro = n // 2

    # Altura proporcional para copa superior, copa inferior y tronco
    fin_copa_sup = int(n * 0.38)
    fin_copa_inf = int(n * 0.78)
    ancho_tronco = max(2, n // 6)
    if ancho_tronco % 2 != n % 2:
        ancho_tronco += 1

    # 1. Copa superior (triángulo)
    for r in range(1, fin_copa_sup + 1):
        radio = int((r / max(1, fin_copa_sup)) * (n * 0.30))
        for c in range(max(0, centro - radio), min(n, centro + radio + 1)):
            matriz[r][c] = 1

    # 2. Copa inferior (triángulo más ancho)
    filas_inf = max(1, fin_copa_inf - fin_copa_sup)
    for idx, r in enumerate(range(fin_copa_sup + 1, fin_copa_inf + 1)):
        radio = int(((idx + 1) / filas_inf) * (n * 0.45))
        for c in range(max(0, centro - radio), min(n, centro + radio + 1)):
            matriz[r][c] = 1

    # 3. Tronco centrado
    inicio_t = max(0, centro - ancho_tronco // 2)
    fin_t = min(n, inicio_t + ancho_tronco)
    for r in range(fin_copa_inf + 1, n - 1):
        for c in range(inicio_t, fin_t):
            matriz[r][c] = 1

    return matriz


def generar_circulo(n):
    matriz = [[0] * n for _ in range(n)]
    centro = (n - 1) / 2.0
    radio = n * 0.36
    for r in range(n):
        for c in range(n):
            if (r - centro) ** 2 + (c - centro) ** 2 <= radio ** 2:
                matriz[r][c] = 1
    return matriz


def generar_cruz(n):
    matriz = [[0] * n for _ in range(n)]
    grosor = max(2, n // 4)
    inicio = (n - grosor) // 2
    fin = inicio + grosor
    margen = max(1, n // 10)
    for r in range(margen, n - margen):
        for c in range(margen, n - margen):
            if inicio <= r < fin or inicio <= c < fin:
                matriz[r][c] = 1
    return matriz


def generar_marco(n):
    matriz = [[0] * n for _ in range(n)]
    borde_ext = max(1, n // 8)
    grosor = max(2, n // 5)
    for r in range(borde_ext, n - borde_ext):
        for c in range(borde_ext, n - borde_ext):
            en_borde_sup_inf = r < borde_ext + grosor or r >= n - borde_ext - grosor
            en_borde_izq_der = c < borde_ext + grosor or c >= n - borde_ext - grosor
            if en_borde_sup_inf or en_borde_izq_der:
                matriz[r][c] = 1
    return matriz


def aplicar_ruido(matriz, prob_ruido, semilla=None):
    if semilla is not None:
        random.seed(semilla)
    n = len(matriz)
    for r in range(n):
        for c in range(n):
            if random.random() < prob_ruido:
                matriz[r][c] = 1 - matriz[r][c]  # Invierte 0 <-> 1


def guardar_txt(matriz, ruta_salida):
    n = len(matriz)
    with open(ruta_salida, "w", encoding="utf-8") as f:
        f.write(f"{n}\n")
        for fila in matriz:
            f.write(" ".join(map(str, fila)) + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Generador de matrices binarias NxN para pruebas de suavizado MPI."
    )
    parser.add_argument("-n", type=int, default=16, help="Dimensión N de la matriz NxN (por defecto: 16)")
    parser.add_argument(
        "-f", "--figura",
        choices=["pino", "circulo", "cruz", "marco"],
        default="pino",
        help="Figura base a dibujar (por defecto: pino)"
    )
    parser.add_argument(
        "-r", "--ruido",
        type=float,
        default=0.05,
        help="Probabilidad de ruido entre 0.0 y 1.0 (por defecto: 0.05 = 5%%)"
    )
    parser.add_argument("-o", "--salida", default="entrada.txt", help="Archivo de salida (por defecto: entrada.txt)")
    parser.add_argument("--seed", type=int, default=None, help="Semilla opcional para reproducibilidad")
    parser.add_argument("--preview", action="store_true", help="Muestra la figura en consola usando '.' y '#'")

    args = parser.parse_args()

    generadores = {
        "pino": generar_pino,
        "circulo": generar_circulo,
        "cruz": generar_cruz,
        "marco": generar_marco,
    }

    matriz = generadores[args.figura](args.n)
    if args.ruido > 0:
        aplicar_ruido(matriz, args.ruido, args.seed)

    guardar_txt(matriz, args.salida)
    print(f"Archivo '{args.salida}' generado ({args.n}x{args.n}, figura='{args.figura}', ruido={args.ruido*100:.1f}%).")

    if args.preview and args.n <= 60:
        print("\nVista previa:")
        for fila in matriz:
            print("".join("#" if val == 1 else "." for val in fila))


if __name__ == "__main__":
    main()