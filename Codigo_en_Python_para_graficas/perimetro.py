import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from scipy.optimize import curve_fit

# Definir el directorio donde se encuentran los archivos
directorio = 'Resultados_simulacion/MAIN/0.80/PROMEDIOS-ERRORES/datos.txt.txt'
# Leo los datos del archivo donde la primera columna es el tamaño del sistema n, la segunda columna es el promedio wilsons en logaritmo y la tercera es el error de los datos en el logaritmo
datos = pd.read_csv(directorio, delim_whitespace=True, header=None, names=['n', 'log_wilson', 'error_log_wilson'])
# Extraer las columnas
n = datos['n'].values
log_wilson = datos['log_wilson'].values
error_log_wilson = datos['error_log_wilson'].values

# Defino funciones para plotear y ajustar
def funcion_teorica_log(n, logA, μ):
    """Función teórica en escala logarítmica: log(A) - μ * 4n"""
    return logA - μ * 4 * n

def funcion_teorica_normal(n, A, μ):
    """Función teórica en escala normal: A * exp(-μ * 4n)"""
    return A * np.exp(-μ * 4 * n)

# Genero una lista de valores de n desde el 0 hasta el 10
ENE = list(range(0, 11))  # Extiendo hasta 10 para incluir el máximo n

# Gráfica en escala logarítmica
plt.figure()  # Nueva figura para el primer gráfico
# Ajuste en escala logarítmica
popt_log, pcov_log = curve_fit(funcion_teorica_log, n, log_wilson, sigma=error_log_wilson, absolute_sigma=True, p0=[0.0, 0.1])
logA_fit, nu_fit = popt_log
perr_log = np.sqrt(np.diag(pcov_log))  # Errores de los parámetros

# Ploteo los datos con error bars en escala logarítmica
plt.errorbar(n, log_wilson, yerr=error_log_wilson, fmt='o', label=f'Datos de simulación (Ajuste: log(A)={logA_fit:.3f}, μ={nu_fit:.3f})', color='blue', ecolor='lightgray', elinewidth=3, capsize=0)
# Ploteo la función teórica ajustada
plt.plot(ENE, funcion_teorica_log(np.array(ENE), *popt_log), label='Función teórica ajustada', color='red')
# Añado etiquetas y leyenda
plt.xlabel('n')
plt.ylabel(r'$\log\langle W \rangle$')
plt.title(r'$\log\langle W \rangle$(n)')
plt.legend(loc='upper right')
# Guardo la gráfica
if not os.path.exists('Graficas'):
    os.makedirs('Graficas')
plt.savefig('Graficas/MAIN/0.80/perimetro_log.png')
plt.show()  # Muestro la gráfica después de guardarla

# Gráfica en escala normal
plt.figure()  # Nueva figura para el segundo gráfico
# Ajuste en escala normal
wilson_normal = np.exp(log_wilson)
error_wilson_normal = wilson_normal * error_log_wilson  # Propagación del error
popt_normal, pcov_normal = curve_fit(funcion_teorica_normal, n, wilson_normal, sigma=error_wilson_normal, absolute_sigma=True, p0=[1.0, 0.1])
A_fit, nu_fit_normal = popt_normal
perr_normal = np.sqrt(np.diag(pcov_normal))  # Errores de los parámetros

# Ploteo los datos con error bars en escala normal
plt.errorbar(n, wilson_normal, yerr=error_wilson_normal, fmt='o', label=f'Datos de simulación (Ajuste: A={A_fit:.3f}, μ={nu_fit_normal:.3f})', color='blue', ecolor='lightgray', elinewidth=3, capsize=0)
# Ploteo la función teórica ajustada
plt.plot(ENE, funcion_teorica_normal(np.array(ENE), *popt_normal), label='Función teórica ajustada', color='red')
# Añado etiquetas y leyenda
plt.xlabel('n')
plt.ylabel(r'$\langle W \rangle$')
plt.title(r'$\langle W \rangle$(n)')
plt.legend(loc='upper right')
# Guardo la gráfica
plt.savefig('Graficas/MAIN/0.80/perimetro.png')
plt.show()  # Muestro la gráfica después de guardarla

# Imprimo los resultados
print(f"Ajuste en escala logarítmica: log(A) = {logA_fit} ± {perr_log[0]}, μ = {nu_fit} ± {perr_log[1]}")
print(f"Ajuste en escala normal: A = {A_fit} ± {perr_normal[0]}, μ = {nu_fit_normal} ± {perr_normal[1]}")
