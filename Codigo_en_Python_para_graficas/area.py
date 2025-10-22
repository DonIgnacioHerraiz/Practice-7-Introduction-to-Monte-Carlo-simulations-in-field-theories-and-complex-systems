import os
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# CONFIGURACIÓN CORRECTA PARA MATHTEXT
plt.rcParams.update({
    "mathtext.fontset": "cm",  # Computer Modern (como LaTeX)
    "font.family": "serif",
    "font.size": 12,
})

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
plt.errorbar(n2_mean, mean_y, yerr=mean_err, fmt='o', label=r'$\langle W \rangle$')
plt.plot(n_fit**2, exp_func(n_fit, *popt_exp_mean), 'r-', label=f'Ajuste exp: A={A_fit_mean:.3f}, σ={sigma_fit_mean:.3f}')
plt.xlabel('n²')
plt.ylabel(r'$\langle W \rangle$')
plt.title(r'Gráfica: $\langle W \rangle$($n^2$)')
plt.legend()
plt.grid(True)


plt.savefig(os.path.join(carpeta_salida, "grafica2.png"), dpi=300, bbox_inches='tight')
plt.tight_layout()
plt.show() 



# ===============================
# GRÁFICA 3 CORREGIDA - Excluyendo primeros promedios
# ===============================
n_skip = 2  # Saltar los primeros 2 promedios (n=1 y n=2)

plt.figure(figsize=(10, 6))

# Preparar datos ORIGINALES para graficar
log_y_mean = np.log(mean_y)
rel_err_mean = np.clip(mean_err / mean_y, None, 0.3)

# FILTRAR los promedios (excluir primeros n_skip puntos)
log_y_med = log_y_mean[n_skip:]
rel_err_med = rel_err_mean[n_skip:]
n2_med = n2_mean[n_skip:]  # n² de los promedios filtrados
n_med = unique_n[n_skip:]  # n de los promedios filtrados

print(f"Promedios originales: {len(unique_n)}")
print(f"Promedios después de filtrar: {len(n_med)}")
print(f"Valores de n excluidos: {unique_n[:n_skip]}")
print(f"Valores de n incluidos: {n_med}")

# 1. PRIMERO: Filtrar todos los datos individuales para que coincidan con los promedios filtrados
# Crear máscara para datos individuales que tengan n en los promedios filtrados
mask_individuales = np.isin(n_all, n_med)

# Datos individuales filtrados
n2_all_filtrado = n2_all[mask_individuales]
log_y_all_filtrado = np.log(y_all)[mask_individuales]
err_all_filtrado = err_all[mask_individuales]

# Calcular pesos para el ajuste con datos filtrados
weights_filtrado = 1.0 / ((err_all_filtrado / y_all[mask_individuales]) ** 2)

# 2. AJUSTE LINEAL usando solo los datos filtrados
try:
    popt_lin_filtrado, pcov_lin_filtrado = curve_fit(linear_func, n2_all_filtrado, log_y_all_filtrado, 
                                                   sigma=1.0/np.sqrt(weights_filtrado), 
                                                   absolute_sigma=True, 
                                                   p0=(0.0, 0.1))
    logA_fit_filtrado, sigma_lin_fit_filtrado = popt_lin_filtrado
    perr_lin_filtrado = np.sqrt(np.diag(pcov_lin_filtrado))
    
    print("═" * 50)
    print("AJUSTE CON DATOS FILTRADOS:")
    print(f"log(A) = {logA_fit_filtrado:.4f} ± {perr_lin_filtrado[0]:.4f}")
    print(f"σ = {sigma_lin_fit_filtrado:.4f} ± {perr_lin_filtrado[1]:.4f}")
    print("═" * 50)
    
except Exception as e:
    print(f"Error en ajuste filtrado: {e}")
    print("Usando ajuste con todos los datos...")
    logA_fit_filtrado, sigma_lin_filtrado = logA_fit, sigma_lin_fit

# 3. GRAFICAR
# Promedios FILTRADOS
plt.errorbar(n2_med, log_y_med, yerr=rel_err_med, 
             fmt='o', markersize=8, capsize=5, capthick=1.5,
             color='blue', alpha=0.8, linewidth=2,
             label=r'$\log\langle W \rangle$ (promedios)')

# Datos individuales FILTRADOS (opcional, para ver la dispersión)
plt.scatter(n2_all_filtrado, log_y_all_filtrado, 
            alpha=0.2, color='gray', s=15, 
            label='Datos individuales (filtrados)')

# Línea de ajuste CON DATOS FILTRADOS
n2_fit_curve = np.linspace(min(n2_med), max(n2_all), 300)
plt.plot(n2_fit_curve, linear_func(n2_fit_curve, logA_fit_filtrado, sigma_lin_fit_filtrado), 
         'r-', linewidth=2.5,
         label=fr'$\log(A) = {logA_fit_filtrado:.3f}$, $\sigma = {sigma_lin_fit_filtrado:.3f}$')

# Labels
plt.xlabel('$n^2$', fontsize=14)
plt.ylabel(r'$\log\langle W \rangle$', fontsize=14)
plt.title(f'Gráfica: $\log\langle W \rangle$ vs $n^2$ (excluyendo n={list(unique_n[:n_skip])})', fontsize=16)

plt.legend(fontsize=12)
plt.grid(True, alpha=0.3)
plt.tight_layout()

plt.savefig(os.path.join(carpeta_salida, "grafica3_sin_primeros_promedios.png"), dpi=300, bbox_inches='tight')
plt.show()

# 4. COMPARACIÓN CON AJUSTE ORIGINAL (opcional)
print("\nCOMPARACIÓN:")
print(f"Ajuste con todos los datos:    log(A) = {logA_fit:.4f}, σ = {sigma_lin_fit:.4f}")
print(f"Ajuste sin primeros {n_skip} promedios: log(A) = {logA_fit_filtrado:.4f}, σ = {sigma_lin_fit_filtrado:.4f}")
"""
# ===============================
# GRAFICA 4 - Promedios (escala log) + ajuste lineal (CORREGIDO)
# ===============================

def linear_func(n2, logA, sigma):
    #Función lineal para log(Y) = log(A) - sigma * n².
    return logA - sigma * n2

# 1️⃣ PREPARAR DATOS PARA EL AJUSTE LINEAL
log_y_mean = np.log(mean_y)

# Calcular errores en escala logarítmica correctamente
# δ(logY) = δY/Y (aproximación para errores pequeños)
err_log_mean = mean_err / mean_y

# 2️⃣ FILTRAR DATOS PROBLEMÁTICOS
# Eliminar puntos donde el error es demasiado grande o Y es muy pequeño
mask = (err_log_mean < 0.5) & (mean_y > 1e-3) & (log_y_mean > -10)
n2_filtered = n2_mean[mask]
log_y_filtered = log_y_mean[mask]
err_log_filtered = err_log_mean[mask]

print(f"Datos para ajuste: {len(n2_filtered)} puntos de {len(n2_mean)} originales")
print("n² values:", n2_filtered)
print("log(Y) values:", log_y_filtered)

# 3️⃣ AJUSTE LINEAL ROBUSTO
try:
    # Usar pesos = 1/error^2
    weights = 1.0 / (err_log_filtered ** 2)
    
    popt_lin_mean, pcov_lin_mean = curve_fit(
        linear_func,
        n2_filtered,
        log_y_filtered,
        sigma=1.0/np.sqrt(weights),  # scipy usa sigma, no weights directamente
        absolute_sigma=True,
        p0=(2.0, 0.067),  # Valores iniciales basados en tu imagen
        maxfev=5000  # Aumentar iteraciones máximas
    )
    
    logA_fit_mean, sigma_lin_fit_mean = popt_lin_mean
    perr_lin_mean = np.sqrt(np.diag(pcov_lin_mean))
    
    print("═" * 50)
    print("RESULTADOS DEL AJUSTE LINEAL:")
    print(f"log(A) = {logA_fit_mean:.3f} ± {perr_lin_mean[0]:.3f}")
    print(f"σ = {sigma_lin_fit_mean:.3f} ± {perr_lin_mean[1]:.3f}")
    print(f"A = {np.exp(logA_fit_mean):.3f}")
    print("═" * 50)
    
except Exception as e:
    print(f"Error en ajuste con pesos: {e}")
    print("Intentando ajuste sin pesos...")
    
    # Ajuste simple sin pesos como fallback
    popt_lin_mean, pcov_lin_mean = curve_fit(
        linear_func,
        n2_filtered,
        log_y_filtered,
        p0=(2.0, 0.067)
    )
    logA_fit_mean, sigma_lin_fit_mean = popt_lin_mean
    perr_lin_mean = np.sqrt(np.diag(pcov_lin_mean))

# 4️⃣ CREAR GRÁFICA ESTILO IMAGEN DE REFERENCIA
plt.figure(figsize=(10, 7))

# Puntos experimentales con estilo mejorado
plt.errorbar(n2_mean, log_y_mean, yerr=err_log_mean, 
             fmt='o', markersize=8, capsize=5, capthick=1.5,
             color='blue', alpha=0.7, linewidth=1.5,
             label='log(Promedios)')

# Línea de ajuste
n2_fit = np.linspace(min(n2_filtered), max(n2_filtered), 100)
y_fit = linear_func(n2_fit, logA_fit_mean, sigma_lin_fit_mean)

plt.plot(n2_fit, y_fit, 'r-', linewidth=3, 
         label=f'Ajuste lineal: log(A) = {logA_fit_mean:.3f} ± {perr_lin_mean[0]:.3f}\nσ = {sigma_lin_fit_mean:.3f} ± {perr_lin_mean[1]:.3f}')

# Configuración de ejes similar a la imagen de referencia
plt.xlabel('n²', fontsize=14, fontweight='bold')
plt.ylabel('log(Promedio Wilson)', fontsize=14, fontweight='bold')
plt.title('Gráfica 4: Promedios (escala log) + ajuste lineal', fontsize=16, fontweight='bold')

# Ajustar límites de ejes
plt.xlim(0, max(n2_mean) * 1.05)
plt.ylim(min(log_y_mean) - 0.5, max(log_y_mean) + 0.5)

plt.legend(fontsize=11, loc='best', framealpha=0.9)
plt.grid(True, alpha=0.3, linestyle='--')
plt.tight_layout()

# Guardar
plt.savefig(os.path.join(carpeta_salida, "grafica4_corregida.png"), dpi=300, bbox_inches='tight')
plt.show()

# 5️⃣ VERIFICACIÓN ADICIONAL: GRÁFICA CON TODOS LOS PUNTOS
plt.figure(figsize=(10, 7))

# Todos los puntos individuales en escala log
plt.errorbar(n2_all, np.log(y_all), yerr=err_all/y_all, 
             fmt='.', alpha=0.3, label='Todos los puntos (log)')

# Promedios
plt.errorbar(n2_mean, log_y_mean, yerr=err_log_mean,
             fmt='o', markersize=8, capsize=5, color='red',
             label='Promedios', linewidth=2)

# Ajuste
plt.plot(n2_fit, y_fit, 'k-', linewidth=3,
         label=f'Ajuste: σ = {sigma_lin_fit_mean:.3f}')

plt.xlabel('n²', fontsize=14)
plt.ylabel('log(Promedio Wilson)', fontsize=14)
plt.title('Verificación: Todos los puntos + promedios + ajuste', fontsize=16)
plt.legend(fontsize=10)
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(os.path.join(carpeta_salida, "verificacion_completa.png"), dpi=300)
plt.show()

# 6️⃣ ANÁLISIS DE CALIDAD DEL AJUSTE
residuals = log_y_filtered - linear_func(n2_filtered, logA_fit_mean, sigma_lin_fit_mean)
chi_squared = np.sum((residuals / err_log_filtered) ** 2)
dof = len(n2_filtered) - 2

print("\n" + "═" * 50)
print("ANÁLISIS DE CALIDAD DEL AJUSTE:")
print(f"χ² = {chi_squared:.2f}")
print(f"Grados de libertad = {dof}")
print(f"χ²/dof = {chi_squared/dof:.2f}")
print(f"R² = {1 - np.var(residuals)/np.var(log_y_filtered):.3f}")
print("═" * 50)
"""