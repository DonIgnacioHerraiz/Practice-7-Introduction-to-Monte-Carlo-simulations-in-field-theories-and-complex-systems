import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import os

# Configuración de directorios
input_dir = "input"
output_dir = "output"

# Asegurar que existen los directorios
os.makedirs(input_dir, exist_ok=True)
os.makedirs(output_dir, exist_ok=True)

# Leer datos del archivo
file_path = os.path.join(input_dir, "datos.txt.txt")
try:
    data = np.loadtxt(file_path)
    n = data[:, 0]
    logW = data[:, 1]
    error_logW = data[:, 2]
    
    print("Datos cargados correctamente:")
    print(f"n: {n}")
    print(f"logW: {logW}")
    print(f"error_logW: {error_logW}")
    
except Exception as e:
    print(f"Error al leer el archivo: {e}")
    exit()

# Calcular n²
n2 = n**2

# Función para el ajuste lineal
def linear_func(x, a, b):
    return a * x + b

# Realizar el ajuste con pesos (inversos de los errores)
popt, pcov = curve_fit(linear_func, n2, logW, sigma=error_logW, absolute_sigma=True)
a, b = popt
a_err, b_err = np.sqrt(np.diag(pcov))

print(f"\nResultados del ajuste:")
print(f"Pendiente (σ) = {a:.6f} ± {a_err:.6f}")
print(f"Ordenada en origen (logA) = {b:.6f} ± {b_err:.6f}")

# Calcular χ²
logW_pred = linear_func(n2, a, b)
chi2 = np.sum(((logW - logW_pred) / error_logW)**2)
chi2_red = chi2 / (len(logW) - 2)  # grados de libertad = n - 2 parámetros

print(f"\nχ² = {chi2:.4f}")
print(f"χ² reducido = {chi2_red:.4f}")

# Calcular R²
ss_res = np.sum((logW - logW_pred)**2)
ss_tot = np.sum((logW - np.mean(logW))**2)
r2 = 1 - (ss_res / ss_tot)

print(f"R² = {r2:.4f}")

# Crear la gráfica
plt.figure(figsize=(10, 6))
plt.errorbar(n2, logW, yerr=error_logW, fmt='o', capsize=5, label='Datos experimentales', markersize=6)

# Crear la línea de ajuste
n2_fit = np.linspace(min(n2), max(n2), 100)
logW_fit = linear_func(n2_fit, a, b)
plt.plot(n2_fit, logW_fit, 'r-', label=f'Ajuste lineal: log(W) = ({b:.4f} ± {b_err:.4f}) + ({a:.4f} ± {a_err:.4f})·n²')

plt.xlabel('n²', fontsize=12)
plt.ylabel('log(W)', fontsize=12)
plt.title('Ajuste lineal: log(W) vs n²', fontsize=14)
plt.grid(True, alpha=0.3)
plt.legend()

# Añadir información del ajuste en el gráfico
textstr = f'$R^2 = {r2:.4f}$\n$\chi^2 = {chi2:.2f}$\n$\chi^2/v = {chi2_red:.2f}$'
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
plt.gca().text(0.05, 0.95, textstr, transform=plt.gca().transAxes, fontsize=10,
               verticalalignment='top', bbox=props)

# Guardar la gráfica
output_path = os.path.join(output_dir, "ajuste_lineal_logW_vs_n2.png")
plt.savefig(output_path, dpi=300, bbox_inches='tight')
plt.show()

print(f"\nGrÁfica guardada en: {output_path}")

# Guardar resultados en un archivo
results_path = os.path.join(output_dir, "resultados_ajuste.txt")
with open(results_path, 'w') as f:
    f.write("RESULTADOS DEL AJUSTE LINEAL: log(W) vs n²\n")
    f.write("============================================\n")
    f.write(f"Pendiente (σ) = {a:.6f} ± {a_err:.6f}\n")
    f.write(f"Ordenada en origen (logA) = {b:.6f} ± {b_err:.6f}\n")
    f.write(f"χ² = {chi2:.4f}\n")
    f.write(f"χ² reducido = {chi2_red:.4f}\n")
    f.write(f"R² = {r2:.4f}\n")

print(f"Resultados guardados en: {results_path}")