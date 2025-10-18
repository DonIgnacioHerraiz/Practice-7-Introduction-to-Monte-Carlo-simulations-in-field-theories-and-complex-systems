import numpy as np
import matplotlib.pyplot as plt

def plot_std_vs_invsqrtn(filename, variable_num, beta_value=0.80):
    """
    Grafica desviación estándar de la media vs 1/sqrt(N) para una variable específica
    y ajusta a una recta. Optimizado para publicación en LaTeX.
    
    Parameters:
    filename: nombre del archivo con los datos
    variable_num: número de la variable (1, 2, 3, ...)
    beta_value: valor de beta para el título
    """
    
    # Configuración de estilo para publicación
    plt.rcParams.update({
        'font.size': 14,
        'axes.titlesize': 16,
        'axes.labelsize': 14,
        'xtick.labelsize': 12,
        'ytick.labelsize': 12,
        'legend.fontsize': 12,
        'figure.titlesize': 16,
        'lines.linewidth': 2,
        'lines.markersize': 6
    })
    
    # Cargar datos
    try:
        data = np.loadtxt(filename)
    except:
        print(f"Error: No se pudo cargar el archivo {filename}")
        return
    
    # Verificar que el archivo tiene suficientes columnas
    n_columns = data.shape[1]
    required_columns = 2 * variable_num
    if n_columns < required_columns:
        print(f"Error: El archivo tiene {n_columns} columnas, pero se necesitan al menos {required_columns}")
        print("Formato esperado: N_med, Media_Var1, Err_Var1, Media_Var2, Err_Var2, ...")
        return
    
    # Extraer columnas relevantes
    n_medidas = data[:, 0]
    std_media = data[:, 2*(variable_num-1) + 2]  # columna del error
    
    # Calcular 1/sqrt(N)
    inv_sqrt_n = 1 / np.sqrt(n_medidas)
    
    # Crear figura con tamaño adecuado para LaTeX
    fig, ax = plt.subplots(figsize=(8, 6))
    
    # Graficar puntos
    ax.plot(inv_sqrt_n, std_media, 'bo-', markersize=6, linewidth=1.5, 
            label='Datos experimentales', alpha=0.8)
    
    # Ajuste lineal (excluir primeros puntos muy ruidosos)
    mask = (n_medidas >= 100)
    coeffs = np.polyfit(inv_sqrt_n[mask], std_media[mask], 1)
    recta_ajuste = np.poly1d(coeffs)
    
    # Graficar ajuste
    x_fit = np.linspace(inv_sqrt_n.min(), inv_sqrt_n.max(), 100)
    y_fit = recta_ajuste(x_fit)
    ax.plot(x_fit, y_fit, 'r-', linewidth=2.5, 
             label=f'Ajuste lineal: $y = {coeffs[0]:.4f}/\\sqrt{{N}} + {coeffs[1]:.4f}$')
    
    # Configurar gráfica
    ax.set_xlabel('$1/\\sqrt{N}$', fontsize=16)
    ax.set_ylabel('$\\sigma(\\langle W_{10} \\rangle)$', fontsize=16)
    ax.set_title(f'Ajuste del error Wilson $10 \\times 10$ para $\\beta = {beta_value}$', 
                 fontsize=16, pad=20)
    
    # Leyenda con marco
    legend = ax.legend(loc='best', framealpha=0.9, fancybox=True, shadow=True)
    legend.get_frame().set_facecolor('white')
    
    # Grid sutil
    ax.grid(True, alpha=0.3, linestyle='--')
    
    # Ajustar márgenes
    plt.tight_layout()
    
    # Mostrar resultados del ajuste
    print(f"\n{'='*60}")
    print(f"RESULTADOS PARA β = {beta_value}")
    print(f"{'='*60}")
    print(f"Archivo: {filename}")
    print(f"Variable analizada: Wilson {variable_num}×{variable_num}")
    print(f"Pendiente (c) = {coeffs[0]:.6f}")
    print(f"Ordenada en origen (b) = {coeffs[1]:.6f}")
    print(f"Ecuación: σ(N) = {coeffs[0]:.6f}/√N + {coeffs[1]:.6f}")
    
    # Calcular R²
    y_pred = recta_ajuste(inv_sqrt_n[mask])
    ss_res = np.sum((std_media[mask] - y_pred)**2)
    ss_tot = np.sum((std_media[mask] - np.mean(std_media[mask]))**2)
    r_squared = 1 - (ss_res / ss_tot)
    print(f"R² del ajuste = {r_squared:.6f}")
    
    # Añadir texto con resultados en la gráfica
    textstr = '\n'.join((
        f'$c = {coeffs[0]:.4f}$',
        f'$b = {coeffs[1]:.4f}$',
        f'$R^2 = {r_squared:.4f}$'))
    
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
    ax.text(0.05, 0.95, textstr, transform=ax.transAxes, fontsize=12,
            verticalalignment='top', bbox=props)
    
    # Predicción de error para N grande
    N_grande = 100000
    error_predicho = coeffs[0] / np.sqrt(N_grande) + coeffs[1]
    print(f"Error predicho para N={N_grande}: {error_predicho:.8f}")
    
    # Calcular N necesario para un error objetivo
    error_obj = 0.0001  # Error objetivo más realista
    if coeffs[1] < error_obj:
        N_necesario = (coeffs[0] / (error_obj - coeffs[1]))**2
        print(f"N necesario para σ ≤ {error_obj}: {N_necesario:.0f} medidas")
    else:
        print("El error mínimo b es mayor que el error objetivo")
    
    print(f"{'='*60}")
    
    # Guardar la figura con alta resolución para LaTeX
    output_filename = f"ajuste_beta_{beta_value}.png"
    plt.savefig(output_filename, dpi=300, bbox_inches='tight', facecolor='white')
    print(f"Gráfico guardado como: {output_filename}")
    
    plt.show()
    
    return coeffs, r_squared

# =============================================================================
# FUNCIÓN PARA GENERAR AMBAS GRÁFICAS (β = 0.72 y β = 0.80)
# =============================================================================

def generar_graficas_comparativas():
    """
    Genera ambas gráficas para β = 0.72 y β = 0.80
    """
    
    # Configuración común
    plt.rcParams.update({
        'font.size': 14,
        'axes.titlesize': 16,
        'axes.labelsize': 14,
        'xtick.labelsize': 12,
        'ytick.labelsize': 12,
        'legend.fontsize': 12,
        'figure.titlesize': 16
    })
    
    # Datos para cada beta
    configs = [
        {'filename': "MEDIAS_0_72.txt", 'beta': 0.72, 'color': 'blue', 'output': 'raizN.png'},
        {'filename': "MEDIAS_0_80.txt", 'beta': 0.80, 'color': 'red', 'output': 'ajuste_0.80.png'}
    ]
    
    resultados = {}
    
    for config in configs:
        try:
            # Cargar datos
            data = np.loadtxt(config['filename'])
            n_medidas = data[:, 0]
            std_media = data[:, 20]  # Wilson 10x10 (columna 21, índice 20)
            
            # Calcular 1/sqrt(N)
            inv_sqrt_n = 1 / np.sqrt(n_medidas)
            
            # Crear figura
            fig, ax = plt.subplots(figsize=(8, 6))
            
            # Graficar puntos
            ax.plot(inv_sqrt_n, std_media, 'o-', color=config['color'], 
                    markersize=5, linewidth=1.5, label='Datos experimentales', alpha=0.8)
            
            # Ajuste lineal
            mask = (n_medidas >= 100)
            coeffs = np.polyfit(inv_sqrt_n[mask], std_media[mask], 1)
            recta_ajuste = np.poly1d(coeffs)
            
            # Graficar ajuste
            x_fit = np.linspace(inv_sqrt_n.min(), inv_sqrt_n.max(), 100)
            y_fit = recta_ajuste(x_fit)
            ax.plot(x_fit, y_fit, 'k-', linewidth=2.5, 
                     label=f'$y = {coeffs[0]:.4f}/\\sqrt{{N}} + {coeffs[1]:.4f}$')
            
            # Configurar gráfica
            ax.set_xlabel('$1/\\sqrt{N}$', fontsize=16)
            ax.set_ylabel('$\\sigma(\\langle W \\rangle)$', fontsize=16)
            ax.set_title(f'$\\beta = {config["beta"]}$', fontsize=18)
            
            # Leyenda
            legend = ax.legend(loc='best')
            legend.get_frame().set_facecolor('white')
            
            # Grid
            ax.grid(True, alpha=0.3, linestyle='--')
            
            # Texto con resultados
            y_pred = recta_ajuste(inv_sqrt_n[mask])
            ss_res = np.sum((std_media[mask] - y_pred)**2)
            ss_tot = np.sum((std_media[mask] - np.mean(std_media[mask]))**2)
            r_squared = 1 - (ss_res / ss_tot)
            
            textstr = '\n'.join((
                f'$c = {coeffs[0]:.4f}$',
                f'$b = {coeffs[1]:.6f}$',
                f'$R^2 = {r_squared:.4f}$'))
            
            props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
            ax.text(0.05, 0.95, textstr, transform=ax.transAxes, fontsize=12,
                    verticalalignment='top', bbox=props)
            
            plt.tight_layout()
            plt.savefig(config['output'], dpi=300, bbox_inches='tight')
            plt.show()
            
            resultados[config['beta']] = {
                'coeffs': coeffs,
                'r_squared': r_squared,
                'filename': config['output']
            }
            
            print(f"Gráfico para β = {config['beta']} guardado como: {config['output']}")
            
        except Exception as e:
            print(f"Error procesando β = {config['beta']}: {e}")
    
    return resultados

# =============================================================================
# EJECUCIÓN
# =============================================================================

if __name__ == "__main__":
    
    # Opción 1: Gráfica individual para β = 0.72
    print("Generando gráfica para β = 0.72")
    coeficientes, r2 = plot_std_vs_invsqrtn("MEDIAS_0_72.txt", 10, 0.72)
    
    # Opción 2: Generar ambas gráficas (descomenta si necesitas ambas)
    # print("\n" + "="*60)
    # print("GENERANDO GRÁFICAS COMPARATIVAS")
    # print("="*60)
    # resultados = generar_graficas_comparativas()