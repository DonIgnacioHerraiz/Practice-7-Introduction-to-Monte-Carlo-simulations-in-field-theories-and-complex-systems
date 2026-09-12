import matplotlib.pyplot as plt

# Nombre del fichero
fichero = "TESTS/temperatura_cero.txt"

# Leer los números del fichero
with open(fichero, "r") as f:
    numeros = [float(linea.strip()) for linea in f if linea.strip()]

# Crear la gráfica
plt.figure(figsize=(8, 5))
plt.plot(
    numeros,
    marker="o",       # tipo de marcador (círculo)
    linestyle="-",    # línea continua
    markersize=3,     # tamaño más pequeño de los puntos
    linewidth=1,      # grosor de línea
    label="Datos"
)

# Añadir detalles
plt.title("Crecimiento de la energía a temperatura infinita")
plt.xlabel("Paso")
plt.ylabel("Energía")
plt.legend()
plt.grid(True)

# Mostrar la gráfica
plt.show()
