#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"
#include <dirent.h>

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

void guardar_parametros(char* archivo_entrada,int N_sweps_entre_medidas,int N_medidas) {
    char* folder_param;
    if(beta==0.72){
        folder_param = "Resultados_simulacion/CORRELACION/0.72/PARAMETROS";
    }else{
        if(beta==0.80){
            folder_param = "Resultados_simulacion/CORRELACION/0.80/PARAMETROS";
        }
    }
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
    fprintf(fparam, "N_sweps_entre_medidas\t%d\n", N_sweps_entre_medidas);
    fprintf(fparam, "N_medidas\t%d\n", N_medidas);
    fprintf(fparam, "Configuracion inicial:\t%s\n", archivo_entrada);

    fclose(fparam);
    printf("Archivo de parámetros creado: %s\n", filename);
}

// ---------------------------------------------------------------------------
// Calcula la autocorrelación normalizada ρ(t) de una serie O[0..N-1].
// Devuelve un array rho[0..N-1] (el usuario debe liberar la memoria).
// ---------------------------------------------------------------------------
double* autocorrelacion(double *O, int N) {
    double *rho = malloc(N * sizeof(double));
    if (!rho) {
        printf("Error: no se pudo reservar memoria para rho\n");
        exit(1);
    }

    // Calcular promedio
    double mean = 0.0;
    for (int i = 0; i < N; i++) mean += O[i];
    mean /= N;

    // Calcular varianza (C(0))
    double var = 0.0;
    for (int i = 0; i < N; i++) {
        double diff = O[i] - mean;
        var += diff * diff;
    }
    var /= N;
    if (var == 0) {
        printf("Error: varianza nula\n");
        free(rho);
        exit(1);
    }

    // Calcular C(t) y normalizar: rho(t) = C(t)/C(0)
    for (int t = 0; t < N; t++) {
        double C_t = 0.0;
        for (int s = 0; s < N - t; s++) {
            C_t += (O[s] - mean) * (O[s + t] - mean);
        }
        C_t /= (N - t);
        rho[t] = C_t / var;
    }

    return rho;
}

// ---------------------------------------------------------------------------
// Calcula las correlaciones para 11 variables del archivo dado
// ---------------------------------------------------------------------------
void calcular_correlaciones(const char *archivo_entrada, const char *archivo_salida) {
    FILE *fin = fopen(archivo_entrada, "r");
    if (!fin) {
        printf("No se pudo abrir el archivo de entrada %s\n", archivo_entrada);
        return;
    }

    double **vars = malloc(11 * sizeof(double*));
    double *tiempo = NULL;
    for (int i = 0; i < 11; i++) vars[i] = NULL;

    int N = 0;
    while (1) {
        double t, valores[11];
        int leidos = fscanf(fin,
            "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &t,
            &valores[0], &valores[1], &valores[2], &valores[3], &valores[4],
            &valores[5], &valores[6], &valores[7], &valores[8], &valores[9], &valores[10]);

        if (leidos != 12) break;

        tiempo = realloc(tiempo, sizeof(double) * (N + 1));
        tiempo[N] = t;

        for (int j = 0; j < 11; j++) {
            vars[j] = realloc(vars[j], sizeof(double) * (N + 1));
            vars[j][N] = valores[j];
        }
        N++;
    }
    fclose(fin);

    // Calcular autocorrelaciones
    double **corr = malloc(11 * sizeof(double*));
    for (int j = 0; j < 11; j++) {
        corr[j] = autocorrelacion(vars[j], N);
    }

    // Guardar resultados
    FILE *fout = fopen(archivo_salida, "w");
    if (!fout) {
        printf("No se pudo crear el archivo de salida %s\n", archivo_salida);
        return;
    }


    // Escribir
    for (int t = 0; t < N; t++) {
        fprintf(fout, "%d", t);
        for (int j = 0; j < 11; j++) fprintf(fout, "\t%f", corr[j][t]);
        fprintf(fout, "\n");
    }

    fclose(fout);
    printf("Archivo de correlaciones creado: %s\n", archivo_salida);

    // Liberar
    for (int j = 0; j < 11; j++) {
        free(vars[j]);
        free(corr[j]);
    }
    free(vars);
    free(corr);
    free(tiempo);
}

// ---------------------------------------------------------------------------
// Concatenar rutas dinámicamente
// ---------------------------------------------------------------------------
char* concat_path(const char *a, const char *b) {
    char *ruta = malloc(strlen(a) + strlen(b) + 2);
    sprintf(ruta, "%s/%s", a, b);
    return ruta;
}

// ---------------------------------------------------------------------------
// Procesa correlaciones de todos los archivos I_k.txt de una carpeta base
// ---------------------------------------------------------------------------
void procesar_correlaciones(const char *carpeta_base) {
    char *ruta_corr = concat_path(carpeta_base, "CORR");

    DIR *dir = opendir(ruta_corr);
    if (!dir) {
        printf("No se pudo abrir la carpeta %s\n", ruta_corr);
        free(ruta_corr);
        return;
    }

    char **archivos = NULL;
    int n_files = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, "I_") && strstr(entry->d_name, ".txt")) {
            archivos = realloc(archivos, sizeof(char*)*(n_files+1));
            archivos[n_files] = strdup(entry->d_name);
            n_files++;
        }
    }
    closedir(dir);

    if (n_files == 0) {
        printf("No se encontraron archivos I_k.txt\n");
        free(ruta_corr);
        return;
    }

    // Leer el primer archivo para determinar número de pasos
    char *ruta_file = concat_path(ruta_corr, archivos[0]);
    FILE *f = fopen(ruta_file, "r");
    if (!f) { printf("No se pudo abrir %s\n", ruta_file); free(ruta_file); return; }

    double t;
    int N = 0;
    while (fscanf(f, "%lf", &t) == 1) {
        for (int j = 0; j < 11; j++) fscanf(f, "%*lf");
        N++;
    }
    fclose(f);
    free(ruta_file);

    // Reservar espacio dinámico
    double **valores[11];
    for (int j = 0; j < 11; j++) {
        valores[j] = malloc(n_files * sizeof(double*));
        for (int k = 0; k < n_files; k++)
            valores[j][k] = malloc(N * sizeof(double));
    }

    // Leer todos los archivos
    for (int k = 0; k < n_files; k++) {
        ruta_file = concat_path(ruta_corr, archivos[k]);
        f = fopen(ruta_file, "r");
        if (!f) { printf("No se pudo abrir %s\n", ruta_file); continue; }

        for (int i = 0; i < N; i++) {
            double tiempo;
            fscanf(f, "%lf", &tiempo);
            for (int j = 0; j < 11; j++)
                fscanf(f, "%lf", &valores[j][k][i]);
        }
        fclose(f);
        free(ruta_file);
    }

    // Calcular medias y desviaciones
    double *media[11], *desv[11];
    for (int j = 0; j < 11; j++) {
        media[j] = malloc(N*sizeof(double));
        desv[j]  = malloc(N*sizeof(double));
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 11; j++) {
            double sum = 0.0, sum2 = 0.0;
            for (int k = 0; k < n_files; k++) sum += valores[j][k][i];
            media[j][i] = sum / n_files;
            for (int k = 0; k < n_files; k++)
                sum2 += (valores[j][k][i] - media[j][i])*(valores[j][k][i] - media[j][i]);
            desv[j][i] = sqrt(sum2 / n_files);
        }
    }

    // CORR_PROMEDIO.txt
    ruta_file = concat_path(carpeta_base, "CORR_PROMEDIO.txt");
    f = fopen(ruta_file, "w");
    for (int i = 0; i < N; i++) {
        fprintf(f, "%d", i);
        for (int j = 0; j < 11; j++)
            fprintf(f, "\t%.6f\t%.6f", media[j][i], desv[j][i]);
        fprintf(f, "\n");
    }
    fclose(f);
    free(ruta_file);

    // COMPATIBILIDAD.txt
    ruta_file = concat_path(carpeta_base, "COMPATIBILIDAD.txt");
    f = fopen(ruta_file, "w");

    for (int j = 0; j < 11; j++) {
        fprintf(f, "-----------VARIABLE %d-----------\n", j+1);
        fprintf(f, "Intervalos de tiempo en los que la correlación es compatible con 0:\n\n");
        int en_intervalo = 0;
        int inicio = 0;
        for (int i = 0; i < N; i++) {
            if (fabs(media[j][i]) <= desv[j][i]) {
                if (!en_intervalo) { inicio = i; en_intervalo = 1; }
            } else if (en_intervalo) {
                fprintf(f, "[%d, %d]\n", inicio, i-1);
                en_intervalo = 0;
            }
        }
        if (en_intervalo) fprintf(f, "[%d, %d]\n", inicio, N-1);
        fprintf(f, "\n");
    }

    fclose(f);
    free(ruta_file);

    // Liberar
    for (int j = 0; j < 11; j++) {
        for (int k = 0; k < n_files; k++)
            free(valores[j][k]);
        free(valores[j]);
        free(media[j]);
        free(desv[j]);
    }
    for (int k = 0; k < n_files; k++) free(archivos[k]);
    free(archivos);
    free(ruta_corr);
}


int main(){
    inicializa_PR(12345);
    int s[3*L*L*L], plaquetas[3*L*L*L];
    double probabilidades[5];

    /*

    int un_sweep = 3*L*L*L;
    int N_sweps_entre_med = 1;
    int N_medidas = 2000;
    
    vector_cociente_prob(probabilidades);
    inicializa_vectores_de_vecinos();

    // Carpeta base según beta
    char folder_base_entrada[256];
    char folder_base_salida[256];
    
    if(beta == 0.72) {
        snprintf(folder_base_entrada, sizeof(folder_base_entrada), 
                 "Resultados_simulacion/TERMALIZACION/0.72/CONFIGURACION_FINAL");
        snprintf(folder_base_salida, sizeof(folder_base_salida), 
                 "Resultados_simulacion/CORRELACION/0.72");
    } else if(beta == 0.80) {
        snprintf(folder_base_entrada, sizeof(folder_base_entrada), 
                 "Resultados_simulacion/TERMALIZACION/0.80/CONFIGURACION_FINAL");
        snprintf(folder_base_salida, sizeof(folder_base_salida), 
                 "Resultados_simulacion/CORRELACION/0.80");
    } else {
        printf("Beta no válido: %f\n", beta);
        return 1;
    }

    // Procesar todos los archivos I_k.txt en la carpeta de entrada
    int k = 0;
    char input_file_config[256];
    char output_file_evol[256];
    char output_file_corr[256];
    char param_file[256];
    
    while (1) {
        // Construir nombres de archivos
        snprintf(input_file_config, sizeof(input_file_config), 
                 "%s/I_%d.txt", folder_base_entrada, k);
        snprintf(output_file_evol, sizeof(output_file_evol), 
                 "%s/EVOLUCION/I_%d.txt", folder_base_salida, k);
        snprintf(output_file_corr, sizeof(output_file_corr), 
                 "%s/CORR/I_%d.txt", folder_base_salida, k);
        snprintf(param_file, sizeof(param_file), 
                 "%s/PARAMETROS/I_%d.txt", folder_base_salida, k);

        // Verificar si el archivo de entrada existe
        FILE* ftest = fopen(input_file_config, "r");
        if (!ftest) {
            printf("No se encontraron más archivos. Procesados %d archivos.\n", k);
            break;
        }
        fclose(ftest);

        printf("\n=== Procesando archivo I_%d.txt ===\n", k);
        
        // Leer configuración inicial
        lee_configuracionInicial(s, input_file_config);
        dame_plaquetas(s,plaquetas);
        
        // Guardar parámetros
        guardar_parametros(input_file_config, N_sweps_entre_med, N_medidas);
        
        // Ejecutar dinámica de Metropolis
        dinamica_metropolis(N_sweps_entre_med, N_medidas, probabilidades, s, plaquetas
        #ifdef correlacion
            , output_file_evol, param_file
        #endif
        );
        
        // Calcular correlaciones
        calcular_correlaciones(output_file_evol, output_file_corr);
        
        k++;
    }

    */
    procesar_correlaciones("Resultados_simulacion/CORRELACION/0.72");

    return 0;
}