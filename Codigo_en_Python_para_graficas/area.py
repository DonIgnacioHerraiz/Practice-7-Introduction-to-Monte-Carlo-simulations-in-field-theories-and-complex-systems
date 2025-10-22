import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import os
from scipy import stats

def linear_func(n2, logA, sigma):
    """Función lineal para el ajuste: log(Y) = log(A) - sigma * n²"""
    return logA - sigma * n2

def procesar_archivo_y_graficar(nombre_archivo):
    """
    Procesa un archivo con formato: N  MEDIA_N  ERR(MEDIA_N)
    y genera gráfica de log(MEDIA_N) vs N² con ajuste lineal
    """
    
    # Leer los datos
    try:
        datos = np.loadtxt(nombre_archivo)
        n = datos[:, 0]          # Primera columna: N
        media = datos[:, 1]      # Segunda columna: MEDIA_N
        error = datos[:, 2]      # Tercera columna: ERR(MEDIA_N)
        
        # Leer el contenido completo del archivo para guardarlo
        with open(nombre_archivo, 'r', encoding='utf-8') as f:
            contenido_original = f.read().strip()
            
    except Exception as e:
        print(f"❌ Error al leer el archivo: {e}")
        return
    
    # Calcular n² y log(media)
    n2 = n**2
    log_media = np.log(media)
    
    # Calcular error en el logaritmo (propagación de errores)
    # δ(logY) = δY / Y
    error_log = error / media
    
    # Ajuste lineal
    try:
        # Usar pesos = 1/error^2 para el ajuste
        pesos = 1.0 / (error_log ** 2)
        
        # Realizar ajuste
        popt, pcov = curve_fit(linear_func, n2, log_media, 
                              sigma=1.0/np.sqrt(pesos), 
                              absolute_sigma=True,
                              p0=(0.0, 0.1))
        
        logA_ajuste, sigma_ajuste = popt
        error_logA, error_sigma = np.sqrt(np.diag(pcov))
        
        # Calcular R²
        y_pred = linear_func(n2, logA_ajuste, sigma_ajuste)
        ss_res = np.sum((log_media - y_pred) ** 2)
        ss_tot = np.sum((log_media - np.mean(log_media)) ** 2)
        r_cuadrado = 1 - (ss_res / ss_tot)
        
        # CÁLCULOS DEL CHI-CUADRADO
        n_puntos = len(n2)
        grados_libertad = n_puntos - 2  # 2 parámetros ajustados
        
        # Chi-cuadrado
        chi_cuadrado = np.sum(((log_media - y_pred) / error_log) ** 2)
        chi_cuadrado_reducido = chi_cuadrado / grados_libertad
        
        # Valor p del ajuste
        p_value = 1 - stats.chi2.cdf(chi_cuadrado, grados_libertad)
        
        # Incertidumbre promedio de los datos
        incertidumbre_promedio = np.mean(error_log)
        
    except Exception as e:
        print(f"❌ Error en el ajuste: {e}")
        return
    
    # Crear carpeta si no existe
    carpeta_salida = "Graficas/MAIN/0.72"
    os.makedirs(carpeta_salida, exist_ok=True)
    
    # Crear gráfica
    plt.figure(figsize=(10, 6))
    
    # Graficar puntos con errores
    plt.errorbar(n2, log_media, yerr=error_log, 
                 fmt='o', markersize=6, capsize=4, capthick=1.5,
                 color='blue', alpha=0.8, linewidth=1.5,
                 label='Datos experimentales')
    
    # Graficar ajuste lineal
    n2_ajuste = np.linspace(min(n2), max(n2), 300)
    log_media_ajuste = linear_func(n2_ajuste, logA_ajuste, sigma_ajuste)
    
    plt.plot(n2_ajuste, log_media_ajuste, 'r-', linewidth=2,
             label=f'Ajuste lineal: log(A) = {logA_ajuste:.4f} ± {error_logA:.4f}\nσ = {sigma_ajuste:.4f} ± {error_sigma:.4f}')
    
    # Configurar gráfica
    plt.xlabel('$n^2$', fontsize=14)
    plt.ylabel('$\log(\langle W \\rangle)$', fontsize=14)
    plt.title(f'Gráfica: $\log(\langle W \\rangle)$ vs $n^2$\nArchivo: {os.path.basename(nombre_archivo)}', fontsize=16)
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    
    # Añadir texto con parámetros del ajuste
    texto_ajuste = f'$R^2 = {r_cuadrado:.4f}$\n$A = {np.exp(logA_ajuste):.4f}$\n$\chi^2/\\nu = {chi_cuadrado_reducido:.4f}$'
    plt.text(0.05, 0.05, texto_ajuste, transform=plt.gca().transAxes, 
             fontsize=12, bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.8))
    
    plt.tight_layout()
    
    # Guardar gráfica
    nombre_base = os.path.splitext(os.path.basename(nombre_archivo))[0]
    ruta_guardado = os.path.join(carpeta_salida, f"{nombre_base}_ajuste.png")
    plt.savefig(ruta_guardado, dpi=300, bbox_inches='tight')
    plt.show()
    
    # GUARDAR RESULTADOS EN ARCHIVO CHI_CUADRADO.txt
    guardar_resultados_chi_cuadrado(
        nombre_archivo, n_puntos, r_cuadrado, sigma_ajuste, error_sigma,
        chi_cuadrado, chi_cuadrado_reducido, grados_libertad, p_value,
        logA_ajuste, error_logA, incertidumbre_promedio, contenido_original
    )
    
    # Mostrar resultados en consola
    print("═" * 60)
    print("RESULTADOS DEL AJUSTE LINEAL:")
    print("═" * 60)
    print(f"Archivo procesado: {nombre_archivo}")
    print(f"Número de puntos: {n_puntos}")
    print(f"log(A) = {logA_ajuste:.6f} ± {error_logA:.6f}")
    print(f"σ (pendiente) = {sigma_ajuste:.6f} ± {error_sigma:.6f}")
    print(f"A = exp(log(A)) = {np.exp(logA_ajuste):.6f}")
    print(f"R² = {r_cuadrado:.6f}")
    print(f"χ² = {chi_cuadrado:.6f}")
    print(f"χ²/ν = {chi_cuadrado_reducido:.6f} (ν = {grados_libertad})")
    print(f"Valor p = {p_value:.6f}")
    print(f"Gráfica guardada en: {ruta_guardado}")
    print("═" * 60)

def guardar_resultados_chi_cuadrado(nombre_archivo, n_puntos, r_cuadrado, pendiente, error_pendiente,
                                   chi_cuadrado, chi_cuadrado_reducido, grados_libertad, p_value,
                                   logA, error_logA, incertidumbre_promedio, contenido_original):
    """Guarda los resultados del análisis en el archivo CHI_CUADRADO.txt"""
    
    archivo_chi = "CHI_CUADRADO.txt"
    
    # Verificar si el archivo existe para añadir separador
    archivo_existe = os.path.isfile(archivo_chi)
    
    with open(archivo_chi, "a", encoding="utf-8") as f:
        if not archivo_existe:
            f.write("ANÁLISIS DE CHI-CUADRADO - AJUSTES LINEALES\n")
            f.write("=" * 50 + "\n\n")
        
        f.write("─" * 60 + "\n")
        f.write(f"ARCHIVO ANALIZADO: {os.path.basename(nombre_archivo)}\n")
        f.write(f"FECHA: {np.datetime64('now')}\n")
        f.write("─" * 60 + "\n")
        
        # CONTENIDO ORIGINAL DEL ARCHIVO
        f.write("\n--- CONTENIDO DEL ARCHIVO ORIGINAL ---\n")
        f.write("Formato: N  MEDIA_N  ERR(MEDIA_N)\n")
        f.write(contenido_original)
        f.write("\n" + "─" * 40 + "\n")
        
        # Parámetros solicitados
        f.write(f"\nN_puntos: {n_puntos}\n")
        f.write(f"R²: {r_cuadrado:.8f}\n")
        f.write(f"Pendiente (σ): {pendiente:.8f}\n")
        f.write(f"Error de la pendiente: {error_pendiente:.8f}\n")
        
        f.write("\n--- PARÁMETROS DEL CHI-CUADRADO ---\n")
        f.write(f"χ²: {chi_cuadrado:.8f}\n")
        f.write(f"χ² reducido (χ²/ν): {chi_cuadrado_reducido:.8f}\n")
        f.write(f"Grados de libertad (ν): {grados_libertad}\n")
        f.write(f"Valor p: {p_value:.8f}\n")
        
        f.write("\n--- PARÁMETROS DEL AJUSTE ---\n")
        f.write(f"log(A): {logA:.8f} ± {error_logA:.8f}\n")
        f.write(f"A: {np.exp(logA):.8f}\n")
        f.write(f"Incertidumbre promedio de los datos: {incertidumbre_promedio:.8f}\n")
        
        # Interpretación del chi-cuadrado
        f.write("\n--- INTERPRETACIÓN DEL AJUSTE ---\n")
        if chi_cuadrado_reducido < 1:
            f.write("El ajuste es EXCELENTE (χ²/ν < 1)\n")
        elif chi_cuadrado_reducido < 2:
            f.write("El ajuste es BUENO (1 ≤ χ²/ν < 2)\n")
        elif chi_cuadrado_reducido < 3:
            f.write("El ajuste es ACEPTABLE (2 ≤ χ²/ν < 3)\n")
        else:
            f.write("El ajuste es DEFICIENTE (χ²/ν ≥ 3)\n")
            
        if p_value > 0.05:
            f.write("El ajuste es estadísticamente significativo (p > 0.05)\n")
        else:
            f.write("El ajuste NO es estadísticamente significativo (p ≤ 0.05)\n")
        
        f.write("\n" + "=" * 60 + "\n\n")
    
    print(f"📊 Resultados guardados en: {archivo_chi}")

# Función principal
def main():
    print("📊 PROGRAMA DE ANÁLISIS DE DATOS CON AJUSTE LINEAL")
    print("=" * 50)
    
    while True:
        # Pedir nombre del archivo
        nombre_archivo = input("\n📁 Introduce el nombre del archivo (o 'q' para salir): ").strip()
        
        if nombre_archivo.lower() == 'q':
            print("👋 ¡Hasta luego!")
            break
        
        # Verificar si el archivo existe
        if not os.path.isfile(nombre_archivo):
            print(f"❌ El archivo '{nombre_archivo}' no existe.")
            continue
        
        # Procesar archivo
        procesar_archivo_y_graficar(nombre_archivo)

# Versión para usar directamente con un archivo específico
def procesar_archivo_directo(nombre_archivo):
    """Procesa un archivo específico sin interfaz interactiva"""
    if os.path.isfile(nombre_archivo):
        procesar_archivo_y_graficar(nombre_archivo)
    else:
        print(f"❌ El archivo '{nombre_archivo}' no existe.")

# Ejecutar el programa
procesar_archivo_directo("grafica.txt")