import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
from scipy.optimize import curve_fit
from scipy.stats import chi2
from datetime import datetime

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

# Función para calcular chi-cuadrado y p-valor
def calcular_chi_cuadrado(y_obs, y_teo, errores, n_parametros):
    """Calcula chi-cuadrado, chi-cuadrado reducido y p-valor"""
    chi2_val = np.sum(((y_obs - y_teo) / errores) ** 2)
    grados_libertad = len(y_obs) - n_parametros
    chi2_reducido = chi2_val / grados_libertad
    p_valor = 1 - chi2.cdf(chi2_val, grados_libertad)
    return chi2_val, chi2_reducido, p_valor, grados_libertad

# Función para calcular R²
def calcular_r_cuadrado(y_obs, y_teo):
    """Calcula el coeficiente de determinación R²"""
    ss_res = np.sum((y_obs - y_teo) ** 2)
    ss_tot = np.sum((y_obs - np.mean(y_obs)) ** 2)
    return 1 - (ss_res / ss_tot)

# Función para generar reporte
def generar_reporte(n, log_wilson, error_log_wilson, logA_fit, perr_log, nu_fit, 
                   chi2_log, chi2_red_log, p_valor_log, gl_log, archivo_salida):
    """Genera un reporte en el formato especificado"""
    
    # Calcular R²
    log_wilson_teo = funcion_teorica_log(n, logA_fit, nu_fit)
    r_cuadrado = calcular_r_cuadrado(log_wilson, log_wilson_teo)
    
    # Calcular incertidumbre promedio
    incertidumbre_promedio = np.mean(error_log_wilson)
    
    # Determinar interpretación del ajuste
    if chi2_red_log >= 3:
        calidad_ajuste = "DEFICIENTE (χ²/ν ≥ 3)"
        significancia = "NO es estadísticamente significativo (p ≤ 0.05)"
    elif chi2_red_log >= 2:
        calidad_ajuste = "REGULAR (2 ≤ χ²/ν < 3)"
        significancia = "NO es estadísticamente significativo (p ≤ 0.05)" if p_valor_log <= 0.05 else "estadísticamente significativo (p > 0.05)"
    elif chi2_red_log >= 1:
        calidad_ajuste = "BUENO (1 ≤ χ²/ν < 2)"
        significancia = "NO es estadísticamente significativo (p ≤ 0.05)" if p_valor_log <= 0.05 else "estadísticamente significativo (p > 0.05)"
    else:
        calidad_ajuste = "EXCELENTE (χ²/ν < 1)"
        significancia = "estadísticamente significativo (p > 0.05)"
    
    # Crear contenido del reporte
    contenido = f"""============================================================

────────────────────────────────────────────────────────────
ARCHIVO ANALIZADO: {os.path.basename(directorio)}
FECHA: {datetime.now().strftime('%Y-%m-%dT%H:%M:%S')}
────────────────────────────────────────────────────────────

--- CONTENIDO DEL ARCHIVO ORIGINAL ---
Formato: N  log(MEDIA_N)  ERR(log(MEDIA_N))
"""
    # Añadir datos originales
    for i in range(len(n)):
        contenido += f"{int(n[i])}\t{log_wilson[i]:.10f}\t{error_log_wilson[i]:.10f}\n"
    
    contenido += f"""────────────────────────────────────────

N_puntos: {len(n)}
R²: {r_cuadrado:.8f}
Pendiente (σ): {nu_fit:.8f}
Error de la pendiente: {perr_log[1]:.8f}

--- PARÁMETROS DEL CHI-CUADRADO ---
χ²: {chi2_log:.8f}
χ² reducido (χ²/ν): {chi2_red_log:.8f}
Grados de libertad (ν): {gl_log}
Valor p: {p_valor_log:.8f}

--- PARÁMETROS DEL AJUSTE ---
log(A): {logA_fit:.8f} ± {perr_log[0]:.8f}
A: {np.exp(logA_fit):.8f}
Incertidumbre promedio de los datos: {incertidumbre_promedio:.8f}

--- INTERPRETACIÓN DEL AJUSTE ---
El ajuste es {calidad_ajuste}
El ajuste {significancia}

============================================================

"""
    
    # Escribir en el archivo (modo append)
    with open(archivo_salida, 'a', encoding='utf-8') as f:
        f.write(contenido)
    
    return contenido

# Genero una lista de valores de n desde el 0 hasta el 10
ENE = list(range(0, 11))  # Extiendo hasta 10 para incluir el máximo n

# Gráfica en escala logarítmica
plt.figure()  # Nueva figura para el primer gráfico
# Ajuste en escala logarítmica
popt_log, pcov_log = curve_fit(funcion_teorica_log, n, log_wilson, sigma=error_log_wilson, absolute_sigma=True, p0=[0.0, 0.1])
logA_fit, nu_fit = popt_log
perr_log = np.sqrt(np.diag(pcov_log))  # Errores de los parámetros

# Calculo valores teóricos para el ajuste logarítmico
log_wilson_teo = funcion_teorica_log(n, *popt_log)

# Calculo chi-cuadrado para ajuste logarítmico
chi2_log, chi2_red_log, p_valor_log, gl_log = calcular_chi_cuadrado(
    log_wilson, log_wilson_teo, error_log_wilson, len(popt_log)
)

# Ploteo los datos con error bars en escala logarítmica
plt.errorbar(n, log_wilson, yerr=error_log_wilson, fmt='o', 
             label=f'Datos de simulación\nAjuste: log(A)={logA_fit:.3f}, μ={nu_fit:.3f}\nχ²/gl={chi2_red_log:.3f}, p={p_valor_log:.3f}', 
             color='blue', ecolor='lightgray', elinewidth=3, capsize=0)
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
if not os.path.exists('Graficas/MAIN/0.80'):
    os.makedirs('Graficas/MAIN/0.80')
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

# Calculo valores teóricos para el ajuste normal
wilson_normal_teo = funcion_teorica_normal(n, *popt_normal)

# Calculo chi-cuadrado para ajuste normal
chi2_normal, chi2_red_normal, p_valor_normal, gl_normal = calcular_chi_cuadrado(
    wilson_normal, wilson_normal_teo, error_wilson_normal, len(popt_normal)
)

# Ploteo los datos con error bars en escala normal
plt.errorbar(n, wilson_normal, yerr=error_wilson_normal, fmt='o', 
             label=f'Datos de simulación\nAjuste: A={A_fit:.3f}, μ={nu_fit_normal:.3f}\nχ²/gl={chi2_red_normal:.3f}, p={p_valor_normal:.3f}', 
             color='blue', ecolor='lightgray', elinewidth=3, capsize=0)
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

# Generar reporte
directorio_base = os.path.dirname(directorio)
archivo_reporte = os.path.join(directorio_base, 'reporte_analisis.txt')

# Generar el reporte para el ajuste logarítmico (como en tu ejemplo)
reporte = generar_reporte(n, log_wilson, error_log_wilson, logA_fit, perr_log, nu_fit,
                         chi2_log, chi2_red_log, p_valor_log, gl_log, archivo_reporte)

print(f"Reporte guardado en: {archivo_reporte}")
print("El reporte se ha añadido al archivo existente (si existía)")

# Imprimo los resultados detallados
print("=" * 60)
print("RESULTADOS DE LOS AJUSTES")
print("=" * 60)

print("\n--- AJUSTE EN ESCALA LOGARÍTMICA ---")
print(f"log(A) = {logA_fit:.6f} ± {perr_log[0]:.6f}")
print(f"μ = {nu_fit:.6f} ± {perr_log[1]:.6f}")
print(f"χ² = {chi2_log:.6f}")
print(f"Grados de libertad = {gl_log}")
print(f"χ² reducido = {chi2_red_log:.6f}")
print(f"p-valor = {p_valor_log:.6f}")

print("\n--- AJUSTE EN ESCALA NORMAL ---")
print(f"A = {A_fit:.6f} ± {perr_normal[0]:.6f}")
print(f"μ = {nu_fit_normal:.6f} ± {perr_normal[1]:.6f}")
print(f"χ² = {chi2_normal:.6f}")
print(f"Grados de libertad = {gl_normal}")
print(f"χ² reducido = {chi2_red_normal:.6f}")
print(f"p-valor = {p_valor_normal:.6f}")

# Mostrar también el reporte en consola
print("\n" + "=" * 60)
print("REPORTE GENERADO:")
print("=" * 60)
print(reporte)