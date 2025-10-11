import os
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# ===============================
# CONFIGURACIÓN
# ===============================
# Define el nombre de la carpeta
carpeta = "Resultados_simulacion/MAIN/0.72/PROMEDIOS_ERRORES"  # <-- cámbialo según corresponda

# Carpeta donde guardar las gráficas
carpeta_salida = "Graficas/MAIN/0.72"
os.makedirs(carpeta_salida, exist_ok=True)

# ===============================
# FUNCIONES
# ===============================
def exp_func(n, A, sigma):
    """Función exponencial teórica."""
    return A * np.exp(-sigma * n**2)

def linear_func(n2, logA, sigma):
    """Función lineal para log(Y) = log(A) - sigma * n²."""
    return logA - sigma * n2

# ===============================
# LECTURA DE FICHEROS
# ===============================
n_all = []
y_all = []
err_all = []

for fname in os.listdir(carpeta):
    if fname.startswith("resultados_") and fname.endswith(".txt"):
        data = np.loadtxt(os.path.join(carpeta, fname))
        print(fname, data.shape)
        n_all.append(data[:, 0])
        y_all.append(data[:, 1])
        err_all.append(data[:, 2])


# Convertir listas a arrays planos
n_all = np.concatenate(n_all)
y_all = np.concatenate(y_all)
err_all = np.concatenate(err_all)

n2_all = n_all**2  # eje X real
print("Total de puntos:", len(n_all))

# ===============================
# AJUSTE EXPONENCIAL (1 y 2)
# ===============================
popt_exp, pcov_exp = curve_fit(exp_func, n_all, y_all, sigma=err_all, absolute_sigma=True, p0=(1.0, 0.1))
A_fit, sigma_fit = popt_exp

# ===============================
# GRAFICA 1 - Todos los puntos + ajuste exponencial
# ===============================
plt.figure(figsize=(8, 6))
plt.errorbar(n2_all, y_all, yerr=err_all, fmt='o', alpha=0.5, label='Datos')
n_fit = np.linspace(min(n_all), max(n_all), 300)
plt.plot(n_fit**2, exp_func(n_fit, *popt_exp), 'r-', label=f'Ajuste exp: A={A_fit:.3f}, σ={sigma_fit:.3f}')
plt.xlabel('n²')
plt.ylabel('Promedio Wilson')
plt.title('Gráfica 1: Todos los datos + ajuste exponencial')
plt.legend()
plt.grid(True)


plt.savefig(os.path.join(carpeta_salida, "grafica1.png"), dpi=300, bbox_inches='tight')
plt.tight_layout()
plt.show() 

# ===============================
# GRAFICA 2 - Promedio ponderado por n² + ajuste exponencial
# ===============================
# Agrupar por n²
unique_n = np.unique(n_all)
mean_y = []
mean_err = []

for n_val in unique_n:
    mask = n_all == n_val
    weights = 1 / err_all[mask]**2
    y_mean = np.average(y_all[mask], weights=weights)
    y_err = np.sqrt(1 / np.sum(weights))
    mean_y.append(y_mean)
    mean_err.append(y_err)

mean_y = np.array(mean_y)
mean_err = np.array(mean_err)
n2_mean = unique_n**2

# Ajuste sobre los valores medios
popt_exp_mean, pcov_exp_mean = curve_fit(exp_func, unique_n, mean_y, sigma=mean_err, absolute_sigma=True, p0=(1.0, 0.1))
A_fit_mean, sigma_fit_mean = popt_exp_mean

plt.figure(figsize=(8, 6))
plt.errorbar(n2_mean, mean_y, yerr=mean_err, fmt='o', label='Promedios ponderados')
plt.plot(n_fit**2, exp_func(n_fit, *popt_exp_mean), 'r-', label=f'Ajuste exp: A={A_fit_mean:.3f}, σ={sigma_fit_mean:.3f}')
plt.xlabel('n²')
plt.ylabel('Promedio Wilson')
plt.title('Gráfica 2: Promedio ponderado + ajuste exponencial')
plt.legend()
plt.grid(True)


plt.savefig(os.path.join(carpeta_salida, "grafica2.png"), dpi=300, bbox_inches='tight')
plt.tight_layout()
plt.show() 

# ===============================
# GRAFICA 3 - Todos los puntos, escala log(Y), ajuste lineal
# ===============================
log_y = np.log(y_all)
weights_lin = 1 / (err_all / y_all)**2  # propagación del error

popt_lin, pcov_lin = curve_fit(linear_func, n2_all, log_y, sigma=1/np.sqrt(weights_lin), absolute_sigma=True, p0=(0.0, 0.1))
logA_fit, sigma_lin_fit = popt_lin

plt.figure(figsize=(8, 6))
plt.errorbar(n2_all, log_y, yerr=err_all / y_all, fmt='o', alpha=0.5, label='log(Datos)')
plt.plot(n_fit**2, linear_func(n_fit**2, *popt_lin), 'r-', label=f'Ajuste lineal: log(A)={logA_fit:.3f}, σ={sigma_lin_fit:.3f}')
plt.xlabel('n²')
plt.ylabel('log(Promedio Wilson)')
plt.title('Gráfica 3: Todos los datos (escala log) + ajuste lineal')
plt.legend()
plt.grid(True)


plt.savefig(os.path.join(carpeta_salida, "grafica3.png"), dpi=300, bbox_inches='tight')
plt.tight_layout()
plt.show() 


# ===============================
# GRAFICA 4 - Promedios, escala log(Y), ajuste lineal
# ===============================
log_y_mean = np.log(mean_y)
weights_lin_mean = 1 / (mean_err / mean_y)**2

popt_lin_mean, pcov_lin_mean = curve_fit(linear_func, n2_mean, log_y_mean, sigma=1/np.sqrt(weights_lin_mean), absolute_sigma=True, p0=(0.0, 0.1))
logA_fit_mean, sigma_lin_fit_mean = popt_lin_mean

plt.figure(figsize=(8, 6))
plt.errorbar(n2_mean, log_y_mean, yerr=mean_err / mean_y, fmt='o', label='log(Promedios)')
plt.plot(n_fit**2, linear_func(n_fit**2, *popt_lin_mean), 'r-', label=f'Ajuste lineal: log(A)={logA_fit_mean:.3f}, σ={sigma_lin_fit_mean:.3f}')
plt.xlabel('n²')
plt.ylabel('log(Promedio Wilson)')
plt.title('Gráfica 4: Promedios (escala log) + ajuste lineal')
plt.legend()
plt.grid(True)


plt.savefig(os.path.join(carpeta_salida, "grafica4.png"), dpi=300, bbox_inches='tight')
plt.tight_layout()
plt.show() 


