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

#include <stdio.h>
#include <stdlib.h>

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
// Lee un archivo de entrada con formato:
// tiempo  var1  var2  var3
// y escribe en otro archivo:
// tiempo  corr(var1)  corr(var2)  corr(var3)
// ---------------------------------------------------------------------------
void calcular_correlaciones(const char *archivo_entrada, const char *archivo_salida) {
    FILE *fin = fopen(archivo_entrada, "r");
    if (!fin) {
        printf("No se pudo abrir el archivo de entrada %s\n", archivo_entrada);
        return;
    }

    // Leer datos en memoria
    int Nmax = 1000000;
    double *var1 = malloc(Nmax * sizeof(double));
    double *var2 = malloc(Nmax * sizeof(double));
    double *var3 = malloc(Nmax * sizeof(double));
    double *tiempo = malloc(Nmax * sizeof(double));

    if (!var1 || !var2 || !var3 || !tiempo) {
        printf("Error: no se pudo reservar memoria\n");
        return;
    }

    int N = 0;
    while (fscanf(fin, "%lf %lf %lf %lf", &tiempo[N], &var1[N], &var2[N], &var3[N]) == 4) {
        N++;
        if (N >= Nmax) {
            printf("Advertencia: alcanzado Nmax = %d\n", Nmax);
            break;
        }
    }
    fclose(fin);

    // Calcular correlaciones
    double *corr_var1 = autocorrelacion(var1, N);
    double *corr_var2 = autocorrelacion(var2, N);
    double *corr_var3 = autocorrelacion(var3, N);

    // Escribir resultado
    FILE *fout = fopen(archivo_salida, "w");
    if (!fout) {
        printf("No se pudo crear el archivo de salida %s\n", archivo_salida);
        return;
    }

    // Escribimos los desfases de autocorrelación como tiempo 0..N-1
    for (int t = 0; t < N; t++) {
        fprintf(fout, "%d\t%f\t%f\t%f\n", t, corr_var1[t], corr_var2[t], corr_var3[t]);
    }

    fclose(fout);
    printf("Archivo de correlaciones creado: %s\n", archivo_salida);

    // Liberar memoria
    free(var1);
    free(var2);
    free(var3);
    free(tiempo);
    free(corr_var1);
    free(corr_var2);
    free(corr_var3);
}


// Función para concatenar rutas dinámicamente
char* concat_path(const char *a, const char *b) {
    char *ruta = malloc(strlen(a) + strlen(b) + 2);
    sprintf(ruta, "%s/%s", a, b);
    return ruta;
}

// Función principal
void procesar_correlaciones(const char *carpeta_base) {
    char *ruta_corr = concat_path(carpeta_base, "CORR");

    DIR *dir = opendir(ruta_corr);
    if (!dir) {
        printf("No se pudo abrir la carpeta %s\n", ruta_corr);
        free(ruta_corr);
        return;
    }

    // Lista dinámica de archivos
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

    // Leer el primer archivo para determinar N (número de tiempos)
    char *ruta_file = concat_path(ruta_corr, archivos[0]);
    FILE *f = fopen(ruta_file, "r");
    if (!f) { printf("No se pudo abrir %s\n", ruta_file); free(ruta_file); return; }

    double *tiempo = NULL;
    double t;
    int N = 0;
    while (fscanf(f, "%lf %*f %*f %*f", &t) == 1) {
        tiempo = realloc(tiempo, sizeof(double)*(N+1));
        tiempo[N] = t;
        N++;
    }
    fclose(f);
    free(ruta_file);

    // Arrays para almacenar todas las correlaciones
    double **plaquetas = malloc(n_files*sizeof(double*));
    double **wilsons   = malloc(n_files*sizeof(double*));
    double **aristas   = malloc(n_files*sizeof(double*));

    for (int k = 0; k < n_files; k++) {
        plaquetas[k] = malloc(N*sizeof(double));
        wilsons[k]   = malloc(N*sizeof(double));
        aristas[k]   = malloc(N*sizeof(double));
    }

    // Leer todos los archivos
    for (int k = 0; k < n_files; k++) {
        ruta_file = concat_path(ruta_corr, archivos[k]);
        f = fopen(ruta_file, "r");
        if (!f) { printf("No se pudo abrir %s\n", ruta_file); continue; }
        for (int i = 0; i < N; i++) {
            fscanf(f, "%*lf %lf %lf %lf", &plaquetas[k][i], &wilsons[k][i], &aristas[k][i]);
        }
        fclose(f);
        free(ruta_file);
    }

    // Calcular media y desvío estándar por tiempo y variable
    double *media_pla = malloc(N*sizeof(double));
    double *media_wil = malloc(N*sizeof(double));
    double *media_ari = malloc(N*sizeof(double));
    double *desv_pla  = malloc(N*sizeof(double));
    double *desv_wil  = malloc(N*sizeof(double));
    double *desv_ari  = malloc(N*sizeof(double));

    for (int i = 0; i < N; i++) {
        double sum, sum2;

        // Plaquetas
        sum = sum2 = 0.0;
        for (int k = 0; k < n_files; k++) { sum += plaquetas[k][i]; }
        media_pla[i] = sum/n_files;
        for (int k = 0; k < n_files; k++) { sum2 += (plaquetas[k][i]-media_pla[i])*(plaquetas[k][i]-media_pla[i]); }
        desv_pla[i] = sqrt(sum2/n_files);

        // Wilsons
        sum = sum2 = 0.0;
        for (int k = 0; k < n_files; k++) { sum += wilsons[k][i]; }
        media_wil[i] = sum/n_files;
        for (int k = 0; k < n_files; k++) { sum2 += (wilsons[k][i]-media_wil[i])*(wilsons[k][i]-media_wil[i]); }
        desv_wil[i] = sqrt(sum2/n_files);

        // Aristas
        sum = sum2 = 0.0;
        for (int k = 0; k < n_files; k++) { sum += aristas[k][i]; }
        media_ari[i] = sum/n_files;
        for (int k = 0; k < n_files; k++) { sum2 += (aristas[k][i]-media_ari[i])*(aristas[k][i]-media_ari[i]); }
        desv_ari[i] = sqrt(sum2/n_files);
    }

    // Escribir CORR_PROMEDIO.txt
    ruta_file = concat_path(carpeta_base, "CORR_PROMEDIO.txt");
    f = fopen(ruta_file, "w");
    for (int i = 0; i < N; i++) {
        fprintf(f, "%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\t%.6f\n",
            tiempo[i],
            media_pla[i], desv_pla[i],
            media_wil[i], desv_wil[i],
            media_ari[i], desv_ari[i]);
    }
    fclose(f);
    free(ruta_file);

    // Escribir COMPATIBILIDAD.txt
    ruta_file = concat_path(carpeta_base, "COMPATIBILIDAD.txt");
    f = fopen(ruta_file, "w");

    const char *nombres[3] = {"PLAQUETAS", "WILSONS", "ARISTAS"};
    double *medias[3] = {media_pla, media_wil, media_ari};
    double *desvs[3]  = {desv_pla, desv_wil, desv_ari};

    for (int var = 0; var < 3; var++) {
        fprintf(f, "-----------%s-----------\n", nombres[var]);
        fprintf(f, "Intervalos de tiempo en los que la correlacion es compatible con 0:\n\n");

        int en_intervalo = 0;
        double inicio = 0.0;
        for (int i = 0; i < N; i++) {
            if (fabs(medias[var][i]) <= 2*desvs[var][i]) {
                if (!en_intervalo) { inicio = tiempo[i]; en_intervalo = 1; }
            } else {
                if (en_intervalo) {
                    fprintf(f, "[%.0f,%.0f]\n\n", inicio, tiempo[i-1]);
                    en_intervalo = 0;
                }
            }
        }
        if (en_intervalo) fprintf(f, "[%.0f,%.0f]\n\n", inicio, tiempo[N-1]);
    }

    fclose(f);

    // Liberar memoria
    for (int k = 0; k < n_files; k++) { free(plaquetas[k]); free(wilsons[k]); free(aristas[k]); free(archivos[k]); }
    free(plaquetas); free(wilsons); free(aristas); free(archivos);
    free(media_pla); free(media_wil); free(media_ari);
    free(desv_pla); free(desv_wil); free(desv_ari);
    free(tiempo);
    free(ruta_corr);
}

int main(){
    inicializa_PR(12345);
    int s[3*L*L*L], plaquetas[3*L*L*L];
    double probabilidades[5];

    int un_sweep = 3*L*L*L;
    int N_sweps_entre_med = 1;
    int N_medidas = 20000;
    /*
    
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