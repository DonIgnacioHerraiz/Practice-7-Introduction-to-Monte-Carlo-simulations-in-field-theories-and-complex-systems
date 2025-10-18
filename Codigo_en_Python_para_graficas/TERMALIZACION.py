import os
import matplotlib.pyplot as plt

# CONFIGURACIÓN PARA LATEX
plt.rcParams.update({
    'font.size': 11,
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'xtick.labelsize': 10,
    'ytick.labelsize': 10,
    'legend.fontsize': 10,
    'figure.figsize': (10, 2.5),  # MÁS ANCHA Y MENOS ALTA
    'figure.dpi': 300,
})

# Rutas
input_file = r"Resultados_simulacion\TERMALIZACION\0.72\VENTANAS\COMPATIBILIDAD_0_19.txt"
output_dir = r"Graficas\TERMALIZACION"

# Crear directorio si no existe
os.makedirs(output_dir, exist_ok=True)

# Leer archivo
with open(input_file, 'r') as f:
    lines = f.readlines()

# Procesar bloques de compatibilidad
current_I = None
intervals = []
data = {}

for line in lines:
    line = line.strip()
    if line.startswith("I_") and "Compatible" in line:
        if current_I is not None:
            data[current_I] = intervals
        current_I = int(line.split("_")[1].split(".")[0])
        intervals = []
    elif line.startswith("[") and "," in line:
        start, end = line.strip("[]").split(",")
        intervals.append((int(start), int(end)))

if current_I is not None:
    data[current_I] = intervals

# Generar gráficos de función binaria (TODAS LAS CONFIGURACIONES)
for k, interval_list in data.items():
    tiempo = list(range(525, 40001))
    compat = [0] * len(tiempo)
    
    for start, end in interval_list:
        for t in range(start, end+1):
            if 525 <= t <= 40000:
                compat[t-525] = 1

    fig, ax = plt.subplots(figsize=(10, 2.5))  # FORMATO HORIZONTAL
    
    # Scatter plot más compacto
    ax.scatter(tiempo, compat, color='black', s=0.5, alpha=0.8)
    
    ax.set_ylim(-0.1, 1.1)
    ax.set_xlabel("Tiempo (Sweeps)", fontsize=11)
    ax.set_ylabel("Estabilidad", fontsize=11)
    ax.set_title(f"Evolución de la estabilidad - I_{k} ($\\beta = 0.72$)", fontsize=12)
    
    # SOLO MOSTRAR 0 Y 1 EN EL EJE Y
    ax.set_yticks([0, 1])
    ax.set_yticklabels(['0', '1'])
    
    # Grid sutil
    ax.grid(True, linestyle='--', alpha=0.3)
    
    # Ajustar márgenes
    plt.tight_layout()
    
    # Guardar todas las gráficas
    plt.savefig(os.path.join(output_dir, f"COMPATIBILIDAD_I_{k}.png"), 
                dpi=300, bbox_inches='tight', facecolor='white')
    plt.close()

print(f"Todas las gráficas generadas correctamente en {output_dir}")
print(f"Número de gráficas generadas: {len(data)}")