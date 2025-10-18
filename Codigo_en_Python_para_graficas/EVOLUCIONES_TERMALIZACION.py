import os
import numpy as np
import matplotlib.pyplot as plt
import glob

# Configuración
plt.rcParams.update({
    'font.size': 11,
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'xtick.labelsize': 10,
    'ytick.labelsize': 10,
    'legend.fontsize': 10,
})

# Carpeta con los archivos
base_dir = r"Resultados_simulacion\TERMALIZACION\0.72\VENTANAS\EVOLUCION"
output_dir = r"Graficas\TERMALIZACION\0.72"
os.makedirs(output_dir, exist_ok=True)

# -----------------------------
# Buscar y filtrar archivos I_0.txt a I_18.txt
# -----------------------------
todos_archivos = sorted(glob.glob(os.path.join(base_dir, "I_*.txt")))
archivos = [
    f for f in todos_archivos
    if 0 <= int(os.path.splitext(os.path.basename(f))[0].split('_')[1]) <= 18
]

if not archivos:
    raise FileNotFoundError("No se encontraron archivos I_0.txt a I_18.txt")

# Bucle para todas las variables N de 1 a 11
for n in range(1, 12):
    print(f"Procesando variable W_{n}x{n}...")
    
    # -----------------------------
    # 1) Calcular media y error de la media para todos los archivos
    # -----------------------------
    valores_todos = []

    for archivo in archivos:
        # Saltar la primera fila de cabecera
        data = np.loadtxt(archivo, comments="#", skiprows=1)
        tiempo = data[:, 0]
        
        var_col = 1 + 2*(n-1)
        
        var = data[:, var_col]
        valores_todos.append(var)

    # Convertir a array: shape = (N_archivos, N_tiempos)
    valores_todos = np.array(valores_todos)

    # Media y error de la media
    media = np.mean(valores_todos, axis=0)
    N_conf = valores_todos.shape[0]
    sigma_media = np.sqrt(np.sum((valores_todos - media)**2, axis=0) / (N_conf*(N_conf-1)))

    # -----------------------------
    # 2) Graficar la media con error y los archivos individuales 9,10,12
    # -----------------------------
    archivos_individuales = [
        f for f in archivos
        if int(os.path.splitext(os.path.basename(f))[0].split('_')[1]) in [9,10,12]
    ]

    # GRÁFICA COMPLETA
    plt.figure(figsize=(10,6))

    # Graficar archivos individuales (solo puntos)
    for archivo in archivos_individuales:
        data = np.loadtxt(archivo, comments="#", skiprows=1)
        tiempo = data[:,0]
        var = data[:, 1 + 2*(n-1)]
        err = data[:, 1 + 2*(n-1) + 1]
        nombre = f"I_{os.path.splitext(os.path.basename(archivo))[0].split('_')[1]}"
        plt.errorbar(tiempo, var, yerr=err, fmt='o', markersize=4, label=nombre, alpha=0.7)

    # Graficar media con error (línea continua)
    plt.errorbar(tiempo, media, yerr=sigma_media, fmt='-',
                 color='black', ecolor='gray', elinewidth=1.5, capsize=3,
                 label='Media (todas las configuraciones)')

    # Ajustes de la gráfica
    plt.title(f"Evolución de $W_{{{n}\\times{n}}}$ ($\\beta = 0.72$)", fontsize=14, pad=20)
    plt.xlabel("Tiempo (Sweeps)", fontsize=12, labelpad=10)
    plt.ylabel(f"$\\langle W_{{{n}\\times{n}}} \\rangle$", fontsize=12, labelpad=10)
    plt.xticks(fontsize=11)
    plt.yticks(fontsize=11)
    plt.legend(fontsize=10, loc='upper right', framealpha=0.9,
               fancybox=True, shadow=True, ncol=1)
    plt.grid(True, linestyle='--', alpha=0.3)
    plt.tight_layout()

    # Guardar gráfica completa
    plt.savefig(os.path.join(output_dir, f"W{n}_0.72.png"), dpi=300, bbox_inches='tight')
    plt.close()

    # GRÁFICA ZOOM (desde 320 hasta 800 sweeps)
    plt.figure(figsize=(10,6))

    # Encontrar datos desde tiempo 320 hasta 800
    mask = (tiempo >= 320) & (tiempo <= 800)
    tiempo_zoom = tiempo[mask]
    media_zoom = media[mask]
    sigma_media_zoom = sigma_media[mask]

    # Graficar archivos individuales en zoom
    for archivo in archivos_individuales:
        data = np.loadtxt(archivo, comments="#", skiprows=1)
        tiempo_ind = data[:,0]
        var_ind = data[:, 1 + 2*(n-1)]
        err_ind = data[:, 1 + 2*(n-1) + 1]
        nombre = f"I_{os.path.splitext(os.path.basename(archivo))[0].split('_')[1]}"
        
        mask_ind = (tiempo_ind >= 320) & (tiempo_ind <= 800)
        plt.errorbar(tiempo_ind[mask_ind], var_ind[mask_ind], yerr=err_ind[mask_ind], 
                     fmt='o', markersize=4, label=nombre, alpha=0.7)

    # Graficar media con error en zoom
    plt.errorbar(tiempo_zoom, media_zoom, yerr=sigma_media_zoom, fmt='-',
                 color='black', ecolor='gray', elinewidth=1.5, capsize=3,
                 label='Media (todas las configuraciones)')

    # Ajustes de la gráfica ZOOM
    plt.title(f"Evolución de $W_{{{n}\\times{n}}}$ - Zoom inicial ($\\beta = 0.72$)", fontsize=14, pad=20)
    plt.xlabel("Tiempo (Sweeps)", fontsize=12, labelpad=10)
    plt.ylabel(f"$\\langle W_{{{n}\\times{n}}} \\rangle$", fontsize=12, labelpad=10)
    plt.xticks(fontsize=11)
    plt.yticks(fontsize=11)
    plt.legend(fontsize=10, loc='upper right', framealpha=0.9,
               fancybox=True, shadow=True, ncol=1)
    plt.grid(True, linestyle='--', alpha=0.3)
    plt.xlim(320, 800)  # Forzar límites desde 320 hasta 800
    plt.tight_layout()

    # Guardar gráfica zoom
    plt.savefig(os.path.join(output_dir, f"W{n}_0.72_ZOOM.png"), dpi=300, bbox_inches='tight')
    plt.close()

print(f"Todas las gráficas guardadas en: {output_dir}")
print(f"Total: {11*2} gráficas generadas (11 variables × 2 versiones cada una)")