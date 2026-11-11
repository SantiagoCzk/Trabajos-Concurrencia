# generar_imagen.py
import sys
import random

if len(sys.argv) < 3:
    print("Uso: python generar_imagen.py <n> <archivo_salida.txt>")
    sys.exit(1)

n = int(sys.argv[1])
filename = sys.argv[2]

with open(filename, "w") as f:
    f.write(f"{n}\n")
    for _ in range(n):
        fila = [str(random.choice([0, 1])) for _ in range(n)]
        f.write(" ".join(fila) + "\n")

print(f"Imagen {n}x{n} generada exitosamente en '{filename}'")