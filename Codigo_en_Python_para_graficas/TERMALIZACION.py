import os
import matplotlib.pyplot as plt

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

# Generar gráficos de función binaria
for k, interval_list in data.items():
    tiempo = list(range(525, 40001))
    compat = [0] * len(tiempo)
    
    for start, end in interval_list:
        for t in range(start, end+1):
            if 525 <= t <= 40000:
                compat[t-525] = 1

    plt.figure(figsize=(15,4))
    plt.step(tiempo, compat, where='mid', color='black', linewidth=1.5, label='f(T)')
    
    plt.ylim(-0.1, 1.1)
    plt.xlabel("Tiempo")
    plt.ylabel("Compatibilidad")
    plt.title(f"Compatibilidad Global I_{k}", fontsize=14, fontweight='bold')
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.legend(loc='upper right')
    
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, f"COMPATIBILIDAD_I_{k}.png"))
    plt.close()

print("Gráficas generadas correctamente en", output_dir)
