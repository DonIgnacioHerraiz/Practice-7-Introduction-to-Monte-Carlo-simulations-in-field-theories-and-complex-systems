import numpy as np
import matplotlib.pyplot as plt
import re
import os

def extraer_datos_chi_cuadrado(archivo_chi):
    """
    Extrae los datos del archivo CHI_CUADRADO.txt
    """
    datos = []
    
    with open(archivo_chi, 'r', encoding='utf-8') as f:
        contenido = f.read()
    
    # Dividir por separadores de análisis
    secciones = contenido.split('─' * 60)
    
    for seccion in secciones[1:]:  # Saltar el encabezado
        if not seccion.strip():
            continue
            
        # Extraer datos usando expresiones regulares
        n_puntos = re.search(r'N_puntos:\s*(\d+)', seccion)
        r_cuadrado = re.search(r'R²:\s*([\d.]+)', seccion)
        pendiente = re.search(r'Pendiente \(σ\):\s*([\d.]+)', seccion)
        error_pendiente = re.search(r'Error de la pendiente:\s*([\d.]+)', seccion)
        chi_cuadrado = re.search(r'χ²:\s*([\d.]+)', seccion)
        chi_reducido = re.search(r'χ² reducido \(χ²/ν\):\s*([\d.]+)', seccion)
        grados_libertad = re.search(r'Grados de libertad \(ν\):\s*(\d+)', seccion)
        p_value = re.search(r'Valor p:\s*([\d.]+)', seccion)
        logA = re.search(r'log\(A\):\s*([-\d.]+)', seccion)
        error_logA = re.search(r'log\(A\):\s*[-\d.]+\s*±\s*([\d.]+)', seccion)
        A = re.search(r'A:\s*([\d.]+)', seccion)
        incertidumbre = re.search(r'Incertidumbre promedio de los datos:\s*([\d.]+)', seccion)
        
        if all([n_puntos, r_cuadrado, pendiente, chi_reducido]):
            datos.append({
                'n_puntos': int(n_puntos.group(1)),
                'r_cuadrado': float(r_cuadrado.group(1)),
                'pendiente': float(pendiente.group(1)),
                'error_pendiente': float(error_pendiente.group(1)) if error_pendiente else 0,
                'chi_cuadrado': float(chi_cuadrado.group(1)) if chi_cuadrado else 0,
                'chi_reducido': float(chi_reducido.group(1)),
                'grados_libertad': int(grados_libertad.group(1)) if grados_libertad else 0,
                'p_value': float(p_value.group(1)) if p_value else 0,
                'logA': float(logA.group(1)) if logA else 0,
                'error_logA': float(error_logA.group(1)) if error_logA else 0,
                'A': float(A.group(1)) if A else 0,
                'incertidumbre': float(incertidumbre.group(1)) if incertidumbre else 0
            })
    
    return datos

def crear_graficas_analisis(datos):
    """
    Crea múltiples gráficas de análisis del chi-cuadrado
    """
    if not datos:
        print("❌ No se encontraron datos válidos en el archivo")
        return
    
    # Crear carpeta de salida
    carpeta_salida = "Graficas/MAIN/0.72"
    os.makedirs(carpeta_salida, exist_ok=True)
    
    # Ordenar por número de puntos
    datos.sort(key=lambda x: x['n_puntos'])
    
    n_puntos = [d['n_puntos'] for d in datos]
    chi_reducido = [d['chi_reducido'] for d in datos]
    r_cuadrado = [d['r_cuadrado'] for d in datos]
    pendiente = [d['pendiente'] for d in datos]
    error_pendiente = [d['error_pendiente'] for d in datos]
    p_value = [d['p_value'] for d in datos]
    A = [d['A'] for d in datos]
    incertidumbre = [d['incertidumbre'] for d in datos]
    
    # GRÁFICA 1: Chi-cuadrado reducido vs Número de puntos (DETALLADA)
    plt.figure(figsize=(12, 8))
    plt.semilogy(n_puntos, chi_reducido, 'o-', linewidth=3, markersize=12, 
                color='red', markerfacecolor='white', markeredgewidth=2)
    plt.axhline(y=1, color='green', linestyle='--', linewidth=2, label='χ²/ν = 1 (Ideal)')
    plt.axhline(y=2, color='orange', linestyle='--', linewidth=2, label='χ²/ν = 2 (Bueno)')
    plt.axhline(y=3, color='red', linestyle='--', linewidth=2, label='χ²/ν = 3 (Límite)')
    plt.xlabel('Número de puntos utilizados en el ajuste', fontsize=12)
    plt.ylabel('χ² reducido (escala logarítmica)', fontsize=12)
    plt.title('EVOLUCIÓN DEL CHI-CUADRADO REDUCIDO\ncon el número de puntos', 
              fontsize=14, fontweight='bold')
    plt.legend(fontsize=10)
    plt.grid(True, alpha=0.3)
    
    # Añadir anotaciones detalladas
    for i, (x, y) in enumerate(zip(n_puntos, chi_reducido)):
        color = 'green' if y < 1 else 'orange' if y < 3 else 'red'
        plt.annotate(f'n={x}\nχ²/ν={y:.1f}', (x, y), 
                    textcoords="offset points", xytext=(0,15), 
                    ha='center', fontsize=9, color=color, fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "1_chi_cuadrado_reducido.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 2: R² vs Número de puntos
    plt.figure(figsize=(10, 6))
    plt.plot(n_puntos, r_cuadrado, 's-', linewidth=2, markersize=8, color='blue')
    plt.xlabel('Número de puntos', fontsize=12)
    plt.ylabel('R²', fontsize=12)
    plt.title('Bondad del ajuste: Coeficiente de determinación', fontsize=14, fontweight='bold')
    plt.ylim(0.7, 1.0)
    plt.grid(True, alpha=0.3)
    
    # Añadir valores de R²
    for i, (x, y) in enumerate(zip(n_puntos, r_cuadrado)):
        plt.annotate(f'{y:.3f}', (x, y), textcoords="offset points", 
                    xytext=(0,10), ha='center', fontsize=8)
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "2_r_cuadrado.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 3: Pendiente (σ) vs Número de puntos con barras de error
    plt.figure(figsize=(10, 6))
    plt.errorbar(n_puntos, pendiente, yerr=error_pendiente, 
                 fmt='o-', linewidth=2, markersize=8, capsize=5, color='purple')
    plt.xlabel('Número de puntos', fontsize=12)
    plt.ylabel('Pendiente (σ)', fontsize=12)
    plt.title('Evolución de la pendiente del ajuste', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    
    # Añadir valores de pendiente
    for i, (x, y, err) in enumerate(zip(n_puntos, pendiente, error_pendiente)):
        plt.annotate(f'{y:.3f}', (x, y + err), textcoords="offset points", 
                    xytext=(0,5), ha='center', fontsize=8)
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "3_pendiente.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 4: Valor p vs Número de puntos
    plt.figure(figsize=(10, 6))
    plt.semilogy(n_puntos, p_value, '^-', linewidth=2, markersize=8, color='brown')
    plt.axhline(y=0.05, color='red', linestyle='--', alpha=0.7, label='p = 0.05 (Significativo)')
    plt.xlabel('Número de puntos', fontsize=12)
    plt.ylabel('Valor p (escala log)', fontsize=12)
    plt.title('Significancia estadística: Valor p', fontsize=14, fontweight='bold')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "4_valor_p.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 5: Parámetro A vs Número de puntos
    plt.figure(figsize=(10, 6))
    plt.plot(n_puntos, A, 'd-', linewidth=2, markersize=8, color='green')
    plt.xlabel('Número de puntos', fontsize=12)
    plt.ylabel('Parámetro A', fontsize=12)
    plt.title('Evolución del parámetro A = exp(log(A))', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    
    # Añadir valores de A
    for i, (x, y) in enumerate(zip(n_puntos, A)):
        plt.annotate(f'{y:.3f}', (x, y), textcoords="offset points", 
                    xytext=(0,10), ha='center', fontsize=8)
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "5_parametro_A.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 6: Relación R² vs χ² reducido
    plt.figure(figsize=(10, 6))
    scatter = plt.scatter(r_cuadrado, chi_reducido, c=n_puntos, 
                         s=100, cmap='viridis', alpha=0.7)
    plt.colorbar(scatter, label='Número de puntos')
    plt.xlim(0.7, 1.0)
    plt.yscale('log')
    plt.xlabel('R²', fontsize=12)
    plt.ylabel('χ² reducido (log)', fontsize=12)
    plt.title('RELACIÓN: R² vs χ² REDUCIDO', fontsize=14, fontweight='bold')
    plt.grid(True, alpha=0.3)
    
    # Añadir etiquetas de número de puntos
    for i, (x, y, n) in enumerate(zip(r_cuadrado, chi_reducido, n_puntos)):
        plt.annotate(f'n={n}', (x, y), textcoords="offset points", 
                    xytext=(5,5), fontsize=8)
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "6_relacion_r2_chi2.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 7: Evolución combinada de parámetros clave
    plt.figure(figsize=(12, 8))
    
    # Subplot 1: Parámetros del ajuste
    plt.subplot(2, 1, 1)
    plt.plot(n_puntos, pendiente, 'o-', label='Pendiente (σ)', linewidth=2, markersize=6)
    plt.plot(n_puntos, A, 's-', label='Parámetro A', linewidth=2, markersize=6)
    plt.xlabel('Número de puntos')
    plt.ylabel('Valor del parámetro')
    plt.title('EVOLUCIÓN DE PARÁMETROS CLAVE DEL AJUSTE', fontweight='bold')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Subplot 2: Métricas de calidad
    plt.subplot(2, 1, 2)
    plt.semilogy(n_puntos, chi_reducido, '^-', label='χ² reducido', linewidth=2, markersize=6)
    plt.plot(n_puntos, r_cuadrado, 'd-', label='R²', linewidth=2, markersize=6)
    plt.axhline(y=1, color='red', linestyle='--', alpha=0.5, label='χ²/ν = 1')
    plt.xlabel('Número de puntos')
    plt.ylabel('Métricas de calidad')
    plt.title('EVOLUCIÓN DE MÉTRICAS DE CALIDAD', fontweight='bold')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "7_evolucion_combinada.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # GRÁFICA 8: Resumen completo (6 subplots)
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('ANÁLISIS COMPLETO DEL AJUSTE LINEAL - VARIACIÓN CON NÚMERO DE PUNTOS', 
                 fontsize=16, fontweight='bold')
    
    # 8.1 Chi-cuadrado reducido
    ax1 = axes[0, 0]
    ax1.semilogy(n_puntos, chi_reducido, 'o-', linewidth=2, markersize=8, color='red')
    ax1.axhline(y=1, color='green', linestyle='--', alpha=0.7, label='χ²/ν = 1')
    ax1.axhline(y=3, color='red', linestyle='--', alpha=0.7, label='χ²/ν = 3')
    ax1.set_xlabel('Número de puntos')
    ax1.set_ylabel('χ² reducido (log)')
    ax1.set_title('Calidad del ajuste: χ² reducido')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 8.2 R²
    ax2 = axes[0, 1]
    ax2.plot(n_puntos, r_cuadrado, 's-', linewidth=2, markersize=8, color='blue')
    ax2.set_xlabel('Número de puntos')
    ax2.set_ylabel('R²')
    ax2.set_title('Bondad del ajuste: R²')
    ax2.set_ylim(0.7, 1.0)
    ax2.grid(True, alpha=0.3)
    
    # 8.3 Pendiente
    ax3 = axes[0, 2]
    ax3.errorbar(n_puntos, pendiente, yerr=error_pendiente, 
                 fmt='o-', linewidth=2, markersize=8, capsize=5, color='purple')
    ax3.set_xlabel('Número de puntos')
    ax3.set_ylabel('Pendiente (σ)')
    ax3.set_title('Evolución de la pendiente')
    ax3.grid(True, alpha=0.3)
    
    # 8.4 Valor p
    ax4 = axes[1, 0]
    ax4.semilogy(n_puntos, p_value, '^-', linewidth=2, markersize=8, color='brown')
    ax4.axhline(y=0.05, color='red', linestyle='--', alpha=0.7, label='p = 0.05')
    ax4.set_xlabel('Número de puntos')
    ax4.set_ylabel('Valor p (log)')
    ax4.set_title('Significancia: Valor p')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    # 8.5 Parámetro A
    ax5 = axes[1, 1]
    ax5.plot(n_puntos, A, 'd-', linewidth=2, markersize=8, color='green')
    ax5.set_xlabel('Número de puntos')
    ax5.set_ylabel('Parámetro A')
    ax5.set_title('Parámetro A del ajuste')
    ax5.grid(True, alpha=0.3)
    
    # 8.6 Incertidumbre
    ax6 = axes[1, 2]
    ax6.plot(n_puntos, incertidumbre, 'v-', linewidth=2, markersize=8, color='orange')
    ax6.set_xlabel('Número de puntos')
    ax6.set_ylabel('Incertidumbre promedio')
    ax6.set_title('Calidad de datos: Incertidumbre')
    ax6.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(os.path.join(carpeta_salida, "8_resumen_completo.png"), 
                dpi=300, bbox_inches='tight')
    plt.show()
    
    # Mostrar resumen en consola
    print("═" * 70)
    print("RESUMEN DEL ANÁLISIS DE CHI-CUADRADO")
    print("═" * 70)
    
    mejor_ajuste = min(datos, key=lambda x: x['chi_reducido'])
    peor_ajuste = max(datos, key=lambda x: x['chi_reducido'])
    
    print(f"MEJOR AJUSTE: {mejor_ajuste['n_puntos']} puntos")
    print(f"  χ² reducido: {mejor_ajuste['chi_reducido']:.6f}")
    print(f"  R²: {mejor_ajuste['r_cuadrado']:.6f}")
    print(f"  Pendiente (σ): {mejor_ajuste['pendiente']:.6f} ± {mejor_ajuste['error_pendiente']:.6f}")
    print(f"  Valor p: {mejor_ajuste['p_value']:.6f}")
    
    print(f"\nPEOR AJUSTE: {peor_ajuste['n_puntos']} puntos")
    print(f"  χ² reducido: {peor_ajuste['chi_reducido']:.6f}")
    print(f"  R²: {peor_ajuste['r_cuadrado']:.6f}")
    
    print(f"\nRANGO DE PENDIENTES: {min(pendiente):.6f} - {max(pendiente):.6f}")
    print(f"RANGO DE R²: {min(r_cuadrado):.6f} - {max(r_cuadrado):.6f}")
    print(f"\n📊 Gráficas guardadas en: {carpeta_salida}")
    print("═" * 70)

def main():
    archivo_chi = "CHI_CUADRADO.txt"
    
    if not os.path.isfile(archivo_chi):
        print(f"❌ El archivo {archivo_chi} no existe")
        return
    
    print("📊 ANALIZANDO ARCHIVO CHI_CUADRADO.txt...")
    datos = extraer_datos_chi_cuadrado(archivo_chi)
    
    if datos:
        print(f"✅ Se encontraron {len(datos)} análisis en el archivo")
        crear_graficas_analisis(datos)
    else:
        print("❌ No se pudieron extraer datos del archivo")

if __name__ == "__main__":
    main()