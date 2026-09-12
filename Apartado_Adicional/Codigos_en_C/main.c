#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "parametros.h"

// --- Declaración de funciones auxiliares ---
void calcular_medias_y_error_append_NVAR(const char *input_path, int N, const char *output_path, int NVARS);
int check_dir_exists(const char *path);
int find_max_plain_I_index(const char *folder);
void procesar_carpeta_NVAR(const char *ruta_carpeta, int N, const char *output_path, int NVARS);

// Arrays globales necesarios para las funciones de red
extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

// Copia de archivo binario/texto simple
int copiar_archivo(const char *src, const char *dst) {
    FILE *fs = fopen(src, "r");
    FILE *fd = fopen(dst, "w");
    if (!fs || !fd) {
        if (fs) fclose(fs);
        if (fd) fclose(fd);
        return 0;
    }
    char buf[4096];
    size_t r;
    while ((r = fread(buf, 1, sizeof(buf), fs)) > 0) {
        fwrite(buf, 1, r, fd);
    }
    fclose(fs);
    fclose(fd);
    return 1;
}

int main() {
    // Inicialización básica
    inicializa_PR((int)time(NULL));
    int s[3*L*L*L], plaquetas[3*L*L*L];
    double probabilidades[5];
    
    // Parámetros generales
    int N_sweps_entre_med_base = 1; // Base para multiplicadores
    int N_medidas = 2000000; // Número muy alto, limitado por tiempo (3600s)
    char input_file[512]; 
    int n_runs = 50; // Número de simulaciones a realizar (ajustable)

    // 1. Configuración de rutas según Beta
    char folder_term_final[512];
    char folder_term_ini[512];
    char folder_term_salida[512];
    char folder_out_O[512];
    char folder_out_W[512];

    if (beta == 0.72) {
        sprintf(folder_term_final, "Resultados_simulacion/TERMALIZACION/0.72/CONFIGURACION_FINAL");
        sprintf(folder_term_ini,   "Resultados_simulacion/TERMALIZACION/0.72/CONFIGURACION_INICIAL");
        sprintf(folder_term_salida,"Resultados_simulacion/TERMALIZACION/0.72/EVOLUCION");
    } else {
        sprintf(folder_term_final, "Resultados_simulacion/TERMALIZACION/0.80/CONFIGURACION_FINAL");
        sprintf(folder_term_ini,   "Resultados_simulacion/TERMALIZACION/0.80/CONFIGURACION_INICIAL");
        sprintf(folder_term_salida,"Resultados_simulacion/TERMALIZACION/0.80/EVOLUCION");
    }

    sprintf(folder_out_O, "%s/O", folder_term_salida);
    sprintf(folder_out_W, "%s/W", folder_term_salida);

    // Crear carpetas si no existen
    #ifdef _WIN32
        mkdir(folder_out_O);
        mkdir(folder_out_W);
    #else
        mkdir(folder_out_O, 0777);
        mkdir(folder_out_W, 0777);
    #endif

    printf("=== INICIANDO MAIN RUN (O vs W) ===\n");
    printf("Beta: %.2f | L: %d | Runs: %d\n", beta, L, n_runs);
    /*
    // 2. Bucle Principal de Simulaciones
    for (int run = 0; run < n_runs; run++) {
        
        // A. Selección de configuración aleatoria
        DIR *dir_final = opendir(folder_term_final);
        if (!dir_final) { printf("❌ No se pudo abrir %s\n", folder_term_final); break; }
        
        struct dirent *entry;
        char selected[512] = {0};
        int count = 0;
        
        // Reservoir sampling para elegir un archivo al azar
        while ((entry = readdir(dir_final)) != NULL) {
            if (strncmp(entry->d_name, "I_", 2) == 0 && strstr(entry->d_name, ".txt")) {
                count++;
                if ((int)(fran()*count) == 0) {
                    snprintf(selected, sizeof(selected), "%s/%s", folder_term_final, entry->d_name);
                }
            }
        }
        closedir(dir_final);

        if (strlen(selected) == 0) { printf("⚠️ No se encontraron ficheros en %s\n", folder_term_final); continue; }

        // B. Copiar a configuración inicial con nuevo índice k
        int k = 0;
        char newname[512];
        while (1) {
            snprintf(newname, sizeof(newname), "%s/I_%d.txt", folder_term_ini, k);
            FILE *f = fopen(newname, "r");
            if (f) { fclose(f); k++; } else break;
        }

        if (!copiar_archivo(selected, newname)) {
            printf("❌ Error copiando %s -> %s\n", selected, newname);
            continue;
        }
        printf("--> Run %d: Usando config %s como I_%d.txt\n", run, selected, k);

        // C. Cargar configuración en memoria
        sprintf(input_file, "%s", newname);
        vector_cociente_prob(probabilidades);
        inicializa_vectores_de_vecinos();
        lee_configuracionInicial(s, input_file);
        dame_plaquetas(s, plaquetas);

        // Precalcular tablas
        double tabla_spin[5]; 
        double tabla_bloque[4][4][2];
        precalcula_tabla_spin(beta, tabla_spin);
        promedio_bloque(beta, tabla_bloque);

        // Semilla base para este run (importante para comparar O vs W)
        int seed_run = (int)time(NULL) + run * 1000;

        // --- D. Ejecutar Variable O ---
        // Configuración sweeps O (Estándar o ajustado)
        int N_sweps_O = 35 * 3; // Por defecto o ajustado

        char filename_O[512];
        char param_O[512];
        snprintf(filename_O, sizeof(filename_O), "%s/I_%d_O.txt", folder_out_O, k);
        snprintf(param_O, sizeof(param_O), "%s/../../PARAMETROS/I_%d_O_param.txt", folder_term_salida, k);

        inicializa_PR(seed_run); // Reiniciar semilla
        // OJO: Asumimos que dinamica_metropolis_O acepta el argumento 'tiempo_limite' (3600)
        #ifdef correlacion
            dinamica_metropolis_O(N_sweps_O, N_medidas, probabilidades, s, plaquetas, tabla_spin, tabla_bloque, filename_O, param_O, 3600);
        #else
            dinamica_metropolis_O(N_sweps_O, N_medidas, probabilidades, s, plaquetas, tabla_spin, tabla_bloque, 3600);
        #endif

        // --- E. Ejecutar Variable W ---
        // Configuración sweeps W (Específica por beta)
        int N_sweps_W = 35 * 3; // Default
        if (beta > 0.71 && beta < 0.73) N_sweps_W = 25 * 3;      // Beta 0.72
        else if (beta > 0.79 && beta < 0.81) N_sweps_W = 19 * 3; // Beta 0.80

        char filename_W[512];
        char param_W[512];
        snprintf(filename_W, sizeof(filename_W), "%s/I_%d_W.txt", folder_out_W, k);
        snprintf(param_W, sizeof(param_W), "%s/../../PARAMETROS/I_%d_W_param.txt", folder_term_salida, k);

        // Restaurar estado del sistema para W (importante: recargar s y plaquetas originales)
        lee_configuracionInicial(s, input_file);
        dame_plaquetas(s, plaquetas);
        
        inicializa_PR(seed_run); // Reiniciar semilla (Mismas condiciones de aleatoriedad)
        
        #ifdef correlacion
            dinamica_metropolis_w(N_sweps_W, N_medidas, probabilidades, s, plaquetas, filename_W, param_W, 3600);
        #else
            dinamica_metropolis_w(N_sweps_W, N_medidas, probabilidades, s, plaquetas, filename_W); 
            // Nota: Si tu versión sin correlación no acepta tiempo, quitar el 3600 o filename
        #endif
    }
    */
    // 3. Generación de DATOS_BUENOS
    printf("\n=== GENERANDO DATOS_BUENOS ===\n");
    
    const char *out_O_file = "DATOS_BUENOS_O.txt";
    const char *out_W_file = "DATOS_BUENOS_W.txt";
    const int N_datos = 50000; // Número máximo de líneas a leer por fichero

    // Limpiar archivos previos
    FILE *fp = fopen(out_O_file, "w"); if(fp) fclose(fp);
    fp = fopen(out_W_file, "w"); if(fp) fclose(fp);

    // Variable O tiene 3 columnas de datos (Plaq, O, Mag) -> NVARS = 3
    procesar_carpeta_NVAR(folder_out_O, N_datos, out_O_file, 3);
    
    // Variable W tiene 11 columnas de datos (Plaq, W2...W10, Mag) -> NVARS = 11
    procesar_carpeta_NVAR(folder_out_W, N_datos, out_W_file, 11);

    printf("✅ Proceso completado.\n");
    return 0;
}

// ==========================================
// IMPLEMENTACIÓN DE FUNCIONES AUXILIARES
// ==========================================

void calcular_medias_y_error_append_NVAR(const char *input_path, int N, const char *output_path, int NVARS) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) { printf("❌ Error abriendo %s\n", input_path); return; }

    double *sum = calloc(NVARS, sizeof(double));
    double *sum2 = calloc(NVARS, sizeof(double));
    double *vars = calloc(NVARS, sizeof(double));
    
    if (!sum || !sum2 || !vars) { printf("❌ Memoria insuficiente\n"); return; }

    int n_read = 0;
    double tiempo;

    while (n_read < N && !feof(fin)) {
        int ncol = fscanf(fin, "%lf", &tiempo);
        for (int i = 0; i < NVARS; i++) ncol += fscanf(fin, "%lf", &vars[i]);

        if (ncol != NVARS + 1) break;

        for (int i = 0; i < NVARS; i++) {
            sum[i] += vars[i];
            sum2[i] += vars[i] * vars[i];
        }
        n_read++;
    }
    fclose(fin);

    if (n_read > 0) {
        FILE *fout = fopen(output_path, "a");
        if (fout) {
            fprintf(fout, "%d", n_read); // Primera columna: N (número de medidas)
            for (int i = 0; i < NVARS; i++) {
                double mean = sum[i] / n_read;
                double var = (sum2[i] / n_read) - mean * mean;
                double std_err = (var > 0) ? sqrt(var / n_read) : 0.0;
                fprintf(fout, "\t%.8f\t%.8f", mean, std_err);
            }
            fprintf(fout, "\n");
            fclose(fout);
        }
    }
    
    free(sum); free(sum2); free(vars);
}

void procesar_carpeta_NVAR(const char *ruta_carpeta, int N, const char *output_path, int NVARS) {
    DIR *dir = opendir(ruta_carpeta);
    if (!dir) { printf("⚠️ No se pudo abrir carpeta %s\n", ruta_carpeta); return; }
    
    printf("📁 Procesando %s (NVARS=%d)...\n", ruta_carpeta, NVARS);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "I_", 2) == 0 && strstr(entry->d_name, ".txt")) {
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", ruta_carpeta, entry->d_name);
            calcular_medias_y_error_append_NVAR(fullpath, N, output_path, NVARS);
        }
    }
    closedir(dir);
}

int check_dir_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && (st.st_mode & S_IFDIR));
}

int find_max_plain_I_index(const char *folder) {
    // Función auxiliar si se necesita generar nombres consecutivos
    return 0; // Simplificado para este ejemplo
}