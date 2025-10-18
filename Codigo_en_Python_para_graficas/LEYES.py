import matplotlib.pyplot as plt
import numpy as np
from scipy import stats
import matplotlib.pyplot as plt
import numpy as np
from scipy import stats

def plot_data_with_fit(filename, plot_type='errorbar', add_fit=True, save_plot=False, output_name='plot.png'):
    """
    Lee un archivo TXT con formato X Y ERR(Y) y genera un gráfico con recta de ajuste
    
    Args:
        filename (str): Nombre del archivo TXT
        plot_type (str): Tipo de gráfico ('errorbar', 'scatter', 'line')
        add_fit (bool): Añadir recta de ajuste lineal
        save_plot (bool): Guardar el gráfico como imagen
        output_name (str): Nombre del archivo de salida
    """
    try:
        # Leer los datos del archivo
        data = np.loadtxt(filename)
        
        # Separar las columnas
        x = data[:, 0]
        y = data[:, 1]
        y_err = data[:, 2]
        
        # Crear el gráfico
        plt.figure(figsize=(12, 8))
        
        # Graficar los datos según el tipo especificado
        if plot_type == 'errorbar':
            plt.errorbar(x, y, yerr=y_err, fmt='o', markersize=8, capsize=5, 
                        capthick=2, elinewidth=2, color='blue', alpha=0.7,
                        label='Datos experimentales')
        elif plot_type == 'scatter':
            scatter = plt.scatter(x, y, c=y_err, s=80, cmap='viridis', alpha=0.7, label='Datos experimentales')
            plt.colorbar(scatter, label='Error de Y')
        elif plot_type == 'line':
            plt.plot(x, y, 'o-', linewidth=2, markersize=6, label='Datos experimentales')
            plt.fill_between(x, y - y_err, y + y_err, alpha=0.3, label='Intervalo de error')
        
        # Añadir recta de ajuste si se solicita
        if add_fit:
            # Realizar ajuste lineal considerando los errores
            slope, intercept, r_value, p_value, std_err = stats.linregress(x, y)
            
            # Calcular valores de la recta ajustada
            x_fit = np.linspace(x.min(), x.max(), 100)
            y_fit = slope * x_fit + intercept
            
            # Graficar la recta de ajuste
            plt.plot(x_fit, y_fit, 'r-', linewidth=2, label=f'Ajuste lineal: y = {slope:.4f}x + {intercept:.4f}')
            
            # Calcular coeficiente de determinación R²
            r_squared = r_value**2
            
            # Calcular errores de la predicción
            y_pred = slope * x + intercept
            residuals = y - y_pred
            chi_squared = np.sum((residuals / y_err)**2)
            reduced_chi_squared = chi_squared / (len(x) - 2)  # grados de libertad = n - 2
            
            # Mostrar datos del ajuste en el gráfico
            textstr = '\n'.join((
                f'Pendiente: {slope:.6f} ± {std_err:.6f}',
                f'Intercepto: {intercept:.6f}',
                f'R² = {r_squared:.6f}',
                f'χ²/ν = {reduced_chi_squared:.4f}'))
            
            props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
            plt.text(0.05, 0.95, textstr, transform=plt.gca().transAxes, fontsize=10,
                    verticalalignment='top', bbox=props)
        
        # Personalizar el gráfico
        plt.xlabel('X', fontsize=14)
        plt.ylabel('Y', fontsize=14)
        plt.title('Gráfico de Datos con Ajuste Lineal', fontsize=16)
        plt.grid(True, alpha=0.3)
        plt.legend(fontsize=12)
        
        # Guardar si se solicita
        if save_plot:
            plt.savefig(output_name, dpi=300, bbox_inches='tight')
            print(f"Gráfico guardado como '{output_name}'")
        
        # Mostrar el gráfico
        plt.tight_layout()
        plt.show()
        
        # Imprimir estadísticas detalladas en consola
        print(f"\n{'='*50}")
        print(f"ANÁLISIS DEL ARCHIVO: {filename}")
        print(f"{'='*50}")
        
        print(f"\nESTADÍSTICAS DESCRIPTIVAS:")
        print(f"- Número de puntos: {len(x)}")
        print(f"- Rango de X: {x.min():.6f} a {x.max():.6f}")
        print(f"- Rango de Y: {y.min():.6f} a {y.max():.6f}")
        print(f"- Error promedio de Y: {y_err.mean():.6f}")
        print(f"- Desviación estándar de Y: {y.std():.6f}")
        
        if add_fit:
            print(f"\nRESULTADOS DEL AJUSTE LINEAL:")
            print(f"- Pendiente (m): {slope:.8f} ± {std_err:.8f}")
            print(f"- Intercepto (b): {intercept:.8f}")
            print(f"- Coeficiente de correlación (r): {r_value:.8f}")
            print(f"- Coeficiente de determinación (R²): {r_squared:.8f}")
            print(f"- Error estándar del ajuste: {std_err:.8f}")
            print(f"- Valor p: {p_value:.8f}")
            print(f"- Chi-cuadrado (χ²): {chi_squared:.4f}")
            print(f"- Chi-cuadrado reducido (χ²/ν): {reduced_chi_squared:.4f}")
            
            print(f"\nECUACIÓN DEL AJUSTE:")
            print(f"y = ({slope:.6f} ± {std_err:.6f}) * x + {intercept:.6f}")
            
            # Evaluar calidad del ajuste
            print(f"\nEVALUACIÓN DE LA CALIDAD DEL AJUSTE:")
            if r_squared > 0.95:
                print(f"- R² = {r_squared:.4f} → Excelente ajuste lineal")
            elif r_squared > 0.85:
                print(f"- R² = {r_squared:.4f} → Buen ajuste lineal")
            elif r_squared > 0.70:
                print(f"- R² = {r_squared:.4f} → Ajuste lineal moderado")
            else:
                print(f"- R² = {r_squared:.4f} → Ajuste lineal pobre")
                
            if abs(reduced_chi_squared - 1) < 0.5:
                print(f"- χ²/ν = {reduced_chi_squared:.4f} → Errores bien estimados")
            else:
                print(f"- χ²/ν = {reduced_chi_squared:.4f} → Posible sobre/subestimación de errores")
        
        print(f"\n{'='*50}")
        
        # Devolver los parámetros del ajuste para uso posterior
        if add_fit:
            return {
                'slope': slope,
                'intercept': intercept,
                'r_value': r_value,
                'r_squared': r_squared,
                'p_value': p_value,
                'std_err': std_err,
                'chi_squared': chi_squared,
                'reduced_chi_squared': reduced_chi_squared
            }
        
    except FileNotFoundError:
        print(f"Error: No se encontró el archivo '{filename}'")
        return None
    except Exception as e:
        print(f"Error al procesar el archivo: {e}")
        return None

# Función adicional para hacer predicciones con el modelo ajustado
def predict_with_model(adjustment_results, x_values):
    """
    Realiza predicciones usando el modelo ajustado
    
    Args:
        adjustment_results (dict): Resultados del ajuste lineal
        x_values (array): Valores de X para predecir
    """
    if adjustment_results is None:
        print("No hay resultados de ajuste disponibles")
        return None
    
    slope = adjustment_results['slope']
    intercept = adjustment_results['intercept']
    std_err = adjustment_results['std_err']
    
    predictions = slope * np.array(x_values) + intercept
    errors = std_err * np.array(x_values)  # Error propagado
    
    print(f"\nPREDICCIONES DEL MODELO:")
    for i, x_val in enumerate(x_values):
        print(f"x = {x_val:.4f} → y = {predictions[i]:.6f} ± {errors[i]:.6f}")
    
    return predictions, errors

# Ejemplos de uso:
if __name__ == "__main__":
    # Ejemplo 1: Gráfico básico con ajuste
    results = plot_data_with_fit('DATOS.txt')
    
    # Ejemplo 2: Gráfico personalizado
    # results = plot_data_with_fit('datos.txt', plot_type='errorbar', save_plot=True, output_name='mi_grafico.png')
    
    # Ejemplo 3: Hacer predicciones
    if results is not None:
        new_x = [1.5, 2.5, 3.5]
        predictions = predict_with_model(results, new_x)