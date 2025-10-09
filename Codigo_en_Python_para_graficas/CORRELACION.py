import numpy as np
import matplotlib.pyplot as plt
import os

# Carpeta donde está CORR_PROMEDIO.txt
carpeta_datos = "Resultados_simulacion/CORRELACION/0.72"
archivo_corr = os.path.join(carpeta_datos, "CORR_PROMEDIO.txt")

# Carpeta donde se guardará la gráfica
carpeta_graficas = "Graficas/CORRELACION/0.72"
os.makedirs(carpeta_graficas, exist_ok=True)  # crear carpeta si no existe
archivo_grafica = os.path.join(carpeta_graficas, "CORRELACION_GRAFICA.png")

# Cargar los datos
# Formato esperado:
# tiempo media_var1 desves_var1 media_var2 desves_var2 media_var3 desves_var3
datos = np.loadtxt(archivo_corr)

tiempo = datos[:, 0]  # eje x (N sweeps)
media_var1 = datos[:, 1]
desv_var1  = datos[:, 2]
media_var2 = datos[:, 3]
desv_var2  = datos[:, 4]
media_var3 = datos[:, 5]
desv_var3  = datos[:, 6]

# Crear la figura
plt.figure(figsize=(10, 6))

# Graficar cada variable con barras de error
plt.errorbar(tiempo, media_var1, yerr=desv_var1, fmt='-o', label='Plaquetas', capsize=3)
plt.errorbar(tiempo, media_var2, yerr=desv_var2, fmt='-s', label='Wilsons', capsize=3)
plt.errorbar(tiempo, media_var3, yerr=desv_var3, fmt='-^', label='Aristas', capsize=3)

# Configurar ejes y título
plt.xlabel("N Sweeps")
plt.ylabel("Correlación")
plt.title("Correlación promedio vs tiempo")
plt.legend()
plt.grid(True)

# Guardar figura
plt.tight_layout()
plt.savefig(archivo_grafica, dpi=300)
plt.show()
