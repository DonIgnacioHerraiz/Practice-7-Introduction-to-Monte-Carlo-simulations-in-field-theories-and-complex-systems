import numpy as np
import matplotlib.pyplot as plt

def graficar_log_vs_N2(archivo, titulo="Gráfica log(Media) vs N^2", guardar_en=None):
    """
    Lee un archivo con columnas: N MEDIA_N ERR_N
    y genera una gráfica de log(MEDIA_N) frente a N^2 con barras de error.
    
    Parámetros:
    - archivo: ruta al archivo de datos
    - titulo: título de la gráfica
    - guardar_en: si se indica, guarda la gráfica en este archivo
    """
    # Cargar datos
    datos = np.loadtxt(archivo)
    N = datos[:, 0]
    MEDIA_N = datos[:, 1]
    ERR_N = datos[:, 2]

    # Transformaciones
    log_media = np.log(MEDIA_N)
    N2 = N**2
    err_log_media = ERR_N / MEDIA_N  # Propagación de error: Δ(log(x)) ≈ Δx/x

    # Crear la gráfica
    plt.figure(figsize=(8, 6))
    plt.errorbar(N2, log_media, yerr=err_log_media, fmt='o', capsize=4, label='Datos')
    plt.xlabel(r'$N^2$')
    plt.ylabel(r'$\log(\mathrm{MEDIA}_N)$')
    plt.title(titulo)
    plt.grid(True)
    plt.legend()

    # Guardar o mostrar
    if guardar_en:
        plt.savefig(guardar_en, dpi=300, bbox_inches='tight')
        print(f"Gráfica guardada en {guardar_en}")
    else:
        plt.show()

graficar_log_vs_N2("GRAFICA_PORFIN.txt")  # Solo muestra la gráfica
graficar_log_vs_N2("GRAFICA_PORFIN.txt", guardar_en="grafica.png")  # La guarda en PNG
