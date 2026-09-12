import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.lines import Line2D

# Definir nombres de archivos
file_o = 'DATOS_BUENOS_O.txt'
file_w = 'DATOS_BUENOS_W.txt'

try:
    # Cargar datos
    df_o = pd.read_csv(file_o, sep='\t', header=None)
    df_w = pd.read_csv(file_w, sep='\t', header=None)

    # Extraer datos del Run 0
    run_index = 0
    
    mean_o = df_o.iloc[run_index, 3]
    err_o = df_o.iloc[run_index, 4]
    
    mean_w = df_w.iloc[run_index, 19]
    err_w = df_w.iloc[run_index, 20]

    # Configuración de fuentes (Aumentando tamaños)
    plt.rcParams.update({
        'font.size': 16,        # Texto general
        'axes.titlesize': 20,   # Título del gráfico
        'axes.labelsize': 18,   # Títulos de ejes
        'xtick.labelsize': 16,  # Números eje X
        'ytick.labelsize': 16,  # Números eje Y
        'legend.fontsize': 16   # Leyenda
    })

    # Crear figura
    fig, ax1 = plt.subplots(figsize=(12, 10)) # Un poco más grande para acomodar fuentes

    # --- Gráfica ---
    # Puntos con barras de error
    ax1.errorbar(0, mean_o, yerr=err_o, fmt='o', color='blue', ecolor='blue', 
                 capsize=12, markersize=12, elinewidth=3, markeredgewidth=3)
    ax1.errorbar(1, mean_w, yerr=err_w, fmt='o', color='red', ecolor='red', 
                 capsize=12, markersize=12, elinewidth=3, markeredgewidth=3)

    # Eje X
    ax1.set_xticks([0, 1])
    ax1.set_xticklabels(['Variable O$\'$\n', 'Variable W\n'])
    ax1.set_xlim(-0.5, 1.5)

    # Formateador personalizado para Eje Y
    # Para poner 8.0 x 10^-5 en lugar de 10^-4 arriba
    def sci_format(x, pos):
        if x == 0: return "0"
        exponent = int(np.floor(np.log10(abs(x))))
        coeff = x / 10**exponent
        return r"${:.1f} \times 10^{{{}}}$".format(coeff, exponent)

    ax1.yaxis.set_major_formatter(ticker.FuncFormatter(sci_format))

    # Títulos
    ax1.set_ylabel(r'Valor Esperado $\langle A \rangle$', fontweight='bold') 
    ax1.set_title('Comparación de Observables (Loop 10x10)', fontweight='bold', pad=20)
    
    # Grid
    ax1.grid(True, linestyle='--', alpha=0.5)
    
    # Leyenda
    legend_elements = [
        Line2D([0], [0], marker='o', color='w', label='Variable O$\'$', 
               markerfacecolor='blue', markersize=12),
        Line2D([0], [0], marker='o', color='w', label='Variable W', 
               markerfacecolor='red', markersize=12)
    ]
    ax1.legend(handles=legend_elements, loc='best', title="Leyenda")

    plt.tight_layout()
    plt.savefig('comparacion_run0_final_large.png', dpi=300)
    
    print(f"Gráfica generada: comparacion_run0_final_large.png")

except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()