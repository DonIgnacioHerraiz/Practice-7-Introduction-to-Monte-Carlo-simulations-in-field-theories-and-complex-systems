#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parametros.h"

#define NVAR 11  // número de variables

void calcular_medias_y_error_append(const char *input_path, int N, const char *output_path) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        printf("❌ No se pudo abrir el archivo de entrada: %s\n", input_path);
        return;
    }

    double *sum = calloc(NVAR, sizeof(double));
    double *sum2 = calloc(NVAR, sizeof(double));
    if (!sum || !sum2) {
        printf("❌ Error al reservar memoria.\n");
        fclose(fin);
        free(sum);
        free(sum2);
        return;
    }

    int n_read = 0;
    double tiempo;
    double vars[NVAR];

    while (n_read < N && !feof(fin)) {
        int ncol = fscanf(fin, "%lf", &tiempo);
        for (int i = 0; i < NVAR; i++)
            ncol += fscanf(fin, "%lf", &vars[i]);

        if (ncol != NVAR + 1) break; // línea incompleta

        for (int i = 0; i < NVAR; i++) {
            sum[i]  += vars[i];
            sum2[i] += vars[i] * vars[i];
        }
        n_read++;
    }

    fclose(fin);

    if (n_read == 0) {
        printf("⚠️ No se leyeron filas válidas de %s\n", input_path);
        free(sum);
        free(sum2);
        return;
    }

    FILE *fout = fopen(output_path, "a");
    if (!fout) {
        printf("❌ No se pudo abrir o crear el archivo de salida: %s\n", output_path);
        free(sum);
        free(sum2);
        return;
    }

    // Escribir N como primera columna
    fprintf(fout, "%d", N);

    for (int i = 0; i < NVAR; i++) {
        double mean = sum[i] / n_read;
        double var  = (sum2[i] / n_read) - mean * mean;
        double std_error = (var > 0.0) ? sqrt(var / n_read) : 0.0; // desviación de la media
        fprintf(fout, "\t%.8f\t%.8f", mean, std_error);
    }
    fprintf(fout, "\n");

    fclose(fout);
    free(sum);
    free(sum2);


}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <string.h>

#define NVAR 11

void generar_log_medias(const char *input_path, const char *output_path) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        printf("❌ No se pudo abrir el archivo de entrada: %s\n", input_path);
        return;
    }

    FILE *fout = fopen(output_path, "w");
    if (!fout) {
        printf("❌ No se pudo crear el archivo de salida: %s\n", output_path);
        fclose(fin);
        return;
    }

    fprintf(fout, "# Archivo generado automáticamente a partir de %s\n", input_path);
    fprintf(fout, "# Contiene log(media) y su error propagado (err_log = err/media)\n\n");

    // Reservamos arrays temporales
    double N;
    double media[NVAR];
    double err[NVAR];

    // Leemos fila a fila
    while (!feof(fin)) {
        int leidos = fscanf(fin, "%lf", &N);
        if (leidos != 1) break;

        for (int i = 0; i < NVAR; i++) {
            if (fscanf(fin, "%lf %lf", &media[i], &err[i]) != 2) {
                fclose(fin);
                fclose(fout);
                printf("❌ Error leyendo columnas en %s\n", input_path);
                return;
            }
        }

        // Para cada variable, escribimos su log(media) y error propagado
        for (int i = 0; i < NVAR; i++) {
            fprintf(fout, "------ VARIABLE %d ------\n", i+1);

            if (media[i] <= 0) {
                fprintf(fout, "⚠️ Media no positiva (%.6e), no se puede tomar log\n\n", media[i]);
                continue;
            }

            double logm = log(media[i]);
            double err_log = fabs(err[i] / media[i]);

            fprintf(fout, "log(Media)\t%.10f\n", logm);
            fprintf(fout, "err(log(Media))\t%.10f\n\n", err_log);
        }

        fprintf(fout, "---------------------------------\n\n");
    }

    fclose(fin);
    fclose(fout);
    printf("✅ Archivo creado correctamente: %s\n", output_path);
}

// Función para procesar archivos en una carpeta específica
void procesar_carpeta(const char *ruta_carpeta, int N, const char *output_path) {
    DIR *dir;
    struct dirent *entry;
    
    printf("📁 Procesando carpeta: %s\n", ruta_carpeta);
    
    dir = opendir(ruta_carpeta);
    if (dir == NULL) {
        printf("❌ No se pudo abrir la carpeta: %s\n", ruta_carpeta);
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        // Verificar si el archivo coincide con el patrón I_k.txt donde k es entero
        if (strncmp(entry->d_name, "I_", 2) == 0) {
            char *extension = strrchr(entry->d_name, '.');
            if (extension != NULL && strcmp(extension, ".txt") == 0) {
                // Verificar que el nombre tenga formato I_k.txt
                char *underscore = strchr(entry->d_name + 2, '_');
                if (underscore == NULL) { // Formato I_k.txt sin underscores adicionales
                    char ruta_completa[512];
                    snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", ruta_carpeta, entry->d_name);
                    
                    printf("📊 Procesando archivo: %s\n", ruta_completa);
                    calcular_medias_y_error_append(ruta_completa, N, output_path);
                }
            }
        }
    }
    
    closedir(dir);
}

void promediar_filas_DATOS_BUENOS(const char *input_filename) {
    FILE *fin = fopen(input_filename, "r");
    if (!fin) {
        printf("❌ No se pudo abrir el archivo de entrada: %s\n", input_filename);
        return;
    }

    // Contar número de líneas en el archivo
    int n_lines = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fin) != NULL) {
        n_lines++;
    }
    
    // Volver al inicio del archivo
    rewind(fin);
    
    if (n_lines == 0) {
        printf("⚠️ El archivo %s está vacío\n", input_filename);
        fclose(fin);
        return;
    }

    printf("📊 Leyendo %d líneas de %s\n", n_lines, input_filename);

    // Cada línea tiene: nombre_archivo, N, y para cada variable: mean, error (11 variables * 2 = 22 valores)
    int valores_por_linea = 1 + 1 + NVAR * 2; // nombre_archivo + N + (mean+error)*11
    
    double *sum_means = calloc(NVAR, sizeof(double));
    double *sum_errors_sq = calloc(NVAR, sizeof(double)); // Para error cuadrático
    if (!sum_means || !sum_errors_sq) {
        printf("❌ Error al reservar memoria.\n");
        fclose(fin);
        free(sum_means);
        free(sum_errors_sq);
        return;
    }

    int lineas_procesadas = 0;
    char nombre_archivo[256];
    int N_val;

    // Leer y procesar cada línea
    for (int i = 0; i < n_lines; i++) {
        // Leer nombre del archivo (primera columna)
        if (fscanf(fin, "%255s", nombre_archivo) != 1) {
            printf("❌ Error leyendo nombre de archivo en línea %d\n", i+1);
            break;
        }
        
        // Leer N (segunda columna)
        if (fscanf(fin, "%d", &N_val) != 1) {
            printf("❌ Error leyendo N en línea %d\n", i+1);
            break;
        }

        // Leer los pares mean, error para cada variable
        int lectura_correcta = 1;
        for (int j = 0; j < NVAR; j++) {
            double mean, error;
            if (fscanf(fin, "%lf %lf", &mean, &error) != 2) {
                printf("❌ Error leyendo mean/error para variable %d en línea %d\n", j+1, i+1);
                lectura_correcta = 0;
                break;
            }
            
            sum_means[j] += mean;
            sum_errors_sq[j] += error * error; // Suma de cuadrados para el error
        }

        if (lectura_correcta) {
            lineas_procesadas++;
        } else {
            break;
        }
    }

    fclose(fin);

    if (lineas_procesadas == 0) {
        printf("⚠️ No se pudieron procesar líneas válidas\n");
        free(sum_means);
        free(sum_errors_sq);
        return;
    }

    // Crear archivo de salida
    FILE *fout = fopen("grafica.txt", "w");
    if (!fout) {
        printf("❌ No se pudo crear el archivo de salida: grafica.txt\n");
        free(sum_means);
        free(sum_errors_sq);
        return;
    }

    // Escribir promedios en el archivo de salida
    fprintf(fout, "# Promedio de %d líneas de %s\n", lineas_procesadas, input_filename);
    fprintf(fout, "# Formato: variable_idx mean error\n");
    
    for (int i = 0; i < NVAR; i++) {
        double mean_promedio = sum_means[i] / lineas_procesadas;
        double error_promedio = sqrt(sum_errors_sq[i]) / lineas_procesadas; // Error cuadrático medio
        
        fprintf(fout, "%d\t%.8f\t%.8f\n", i+1, mean_promedio, error_promedio);
    }

    fclose(fout);
    free(sum_means);
    free(sum_errors_sq);

    printf("✅ Archivo 'grafica.txt' creado exitosamente\n");
    printf("📈 Promediadas %d líneas, %d variables por línea\n", lineas_procesadas, NVAR);
}

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];
/*
void guardar_parametros(int k_ini, int N_sweps_entre_medidas,int N_medidas) {
    char* folder_param;
    #ifdef Na
    if (beta == 0.72) {
        folder_param   = "Resultados_simulacion/MAIN/0.72/PARAMETROS/NASIO";
    } else if (beta == 0.8) {
        folder_param   = "Resultados_simulacion/MAIN/0.8/PARAMETROS/NASIO";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Ba)
    if (beta == 0.72) {
        folder_param   = "Resultados_simulacion/MAIN/0.72/PARAMETROS/BARRACHINA";
    } else if (beta == 0.8) {
        folder_param   = "Resultados_simulacion/MAIN/0.8/PARAMETROS/BARRACHINA";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Jo)
    if (beta == 0.72) {
        folder_param   = "Resultados_simulacion/MAIN/0.72/PARAMETROS/JOEL";
    } else if (beta == 0.8) {
        folder_param   = "Resultados_simulacion/MAIN/0.8/PARAMETROS/JOEL";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#else
    #error "Debes definir Na, Ba o Jo antes de compilar"
#endif
    char filename[256];
    int k = 0;
    FILE* ftest;

    // Buscar el índice k más pequeño cuyo archivo no exista
    while (1) {
        snprintf(filename, sizeof(filename), "%s/I_%d.txt", folder_param, k);
        ftest = fopen(filename, "r");
        if (ftest) {
            fclose(ftest);
            k++;
        } else {
            break;
        }
    }

    // Crear el archivo I_k.txt
    FILE* fparam = fopen(filename, "w");
    if (!fparam) {
        printf("No se pudo crear el archivo de parámetros %s\n", filename);
        return;
    }

    // Escribir los parámetros en el formato especificado
    fprintf(fparam, "L\t%d\n", L);
    fprintf(fparam, "J\t%f\n", J);
    fprintf(fparam, "beta\t%f\n", beta);
    fprintf(fparam, "configuracion_inicial: \tI_%d.txt\n",k_ini);

    fprintf(fparam, "N_sweps_entre_medidas\t%d\n", N_sweps_entre_medidas);
    fprintf(fparam, "N_medidas\t%d\n", N_medidas);

    fclose(fparam);
    printf("Archivo de parámetros creado: %s\n", filename);
}
*/

int main(){

    inicializa_PR((int)time(NULL));
    int s[3*L*L*L], plaquetas[3*L*L*L];
    double probabilidades[5];
    int N_sweps_entre_med=35*3;
    int N_medidas=50000;
    char input_file[512]; 

    /*
    for(int j=0;j<100;j++){
    int k_ini=(int)(fran()*20);
    sprintf(input_file, "Resultados_simulacion/MAIN/0.72/CONFIGURACIONES/I_%d.txt", k_ini);
    vector_cociente_prob(probabilidades);
    inicializa_vectores_de_vecinos();
    lee_configuracionInicial(s, input_file);
    dame_plaquetas(s, plaquetas);
    guardar_parametros(k_ini,N_sweps_entre_med,N_medidas);
    dinamica_metropolis_main(N_sweps_entre_med, N_medidas, probabilidades,s, plaquetas,input_file);
    }
    */
    
    const int N = 50000;
    const char *output_path = "DATOS_BUENOS.txt";
    
    // Primero: generar DATOS_BUENOS.txt procesando todas las carpetas
    const char *carpetas[] = {
        "Resultados_simulacion/MAIN/0.72/EVOLUCION/BARRACHINA",
        "Resultados_simulacion/MAIN/0.72/EVOLUCION/JOEL", 
        "Resultados_simulacion/MAIN/0.72/EVOLUCION/JOEL_1",
        "Resultados_simulacion/MAIN/0.72/EVOLUCION/NASIO",
        "Resultados_simulacion/MAIN/0.72/EVOLUCION/NASIO_1"
    };
    
    int num_carpetas = sizeof(carpetas) / sizeof(carpetas[0]);
    
    // Limpiar el archivo de salida al inicio
    FILE *fout = fopen(output_path, "w");
    if (fout) {
        fclose(fout);
        printf("🧹 Archivo de salida limpiado: %s\n", output_path);
    }
    
    // Procesar cada carpeta
    for (int i = 0; i < num_carpetas; i++) {
        procesar_carpeta(carpetas[i], N, output_path);
    }
    
    printf("✅ DATOS_BUENOS.txt generado exitosamente\n");
    
    // Segundo: promediar las filas de DATOS_BUENOS.txt
    promediar_filas_DATOS_BUENOS("DATOS_BUENOS.txt");
    
    
    return 0;
}