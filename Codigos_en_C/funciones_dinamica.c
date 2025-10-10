#include "funciones_dinamica.h"

// Crea una configuración pero no la guarda en un archivo
void crea_configuracion( int flag, int *s){
    int i;
    double r;
    double V=3*L*L*L;

    switch( flag ){
    case 0:
        for( i=0; i<V; i++ ){
            r = fran();
            if( r<0.5 )s[i]=1;
            else s[i]=-1;
            }
        break;

    case 1:
        if( fran() < 0.5 ){
            for( i=0; i<V; i++ ){
                s[i] = -1;
            }
        }
        else{
            for( i=0; i<V; i++ ){
                s[i]= 1;
            }
        }
        break;
    }

    }

// Crea una configuración y la guarda en un archivo
void crea_configuracionInicial( int flag, int *s){
    int i;
    double r;
    double V=3*L*L*L;
    char *filename = (char *)malloc(256 * sizeof(char));
    char* folder;

#ifdef termalizacion
    if(beta == 0.72) {
        folder = "Resultados_simulacion/TERMALIZACION/0.72/CONFIGURACION_INICIAL";
    }
    else if(beta == 0.8) {
        folder = "Resultados_simulacion/TERMALIZACION/0.80/CONFIGURACION_INICIAL";
    }
    else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO");
        // Es buena idea asignar un valor por defecto
        folder = "Resultados_simulacion/CONFIGURACION_INICIAL";
    }
    #else 
    folder = "Resultados_simulacion/CONFIGURACION_INICIAL";
    #endif

    FILE* file;
    int k = 0;

    // Buscamos el primer I_k.txt que no exista
    while (1) {
        snprintf(filename, 256, "%s/I_%d.txt", folder, k);
        file = fopen(filename, "r");
        if (file) {
            // El archivo existe, cerramos y probamos el siguiente
            fclose(file);
            k++;
        } else {
            // No existe, podemos usar este k
            break;
        }
    }


    // Creamos el archivo para escritura
    file = fopen(filename, "w");
    if (!file) {
        printf("No se pudo crear el archivo %s\n", filename);
        filename[0] = '\0'; // indicamos fallo
        return;
    }

    switch( flag ){
    case 0: // ALEATORIA
        for( i=0; i<V; i++ ){
            r = fran();
            if( r<0.5 )s[i]=1;
            else s[i]=-1;
            }
        break;

    case 1: // TODAS IGUALES
        if( fran() < 0.5 ){
            for( i=0; i<V; i++ ){
                s[i] = -1;
            }
        }
        else{
            for( i=0; i<V; i++ ){
                s[i]= 1;
            }
        }
        break;
    }

    for( i=0; i<V; i++ ){
            fprintf(file, "%d\n", s[i]);
        }

    fclose(file);
    }

    
void lee_configuracionInicial(int *s, char *input_file){
    int i;
    double r;
    double V=3*L*L*L;
    FILE *fp;
    fp = fopen(input_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file %s\n", input_file);
        exit(EXIT_FAILURE);
    }else{
        for( i=0; i<V; i++ ){
            fscanf(fp, "%d", &s[i]);
        }
    }
        fclose(fp);
}



int arista_aleatoria(){
    int V=3*L*L*L;
    return (int)(V*fran());
}

int nodo_aleatorio(){
    int V=L*L*L;
    return (int)(V*fran());
}

int cambio_aristas_nodo(int *s){
    int nodo=(int)(fran()*(L*L*L));
    
    s[3*nodo]*=-1;
    s[3*nodo+1]*=-1;
    s[3*nodo+2]*=-1;
    s[3*(nodo+xm[nodo])]*=-1;
    s[3*(nodo+ym[nodo])+1]*=-1;
    s[3*(nodo+zm[nodo])+2]*=-1;
    return nodo;
}
void posicion_plaquetas(int arista_aleatoria, int *posiciones){
    int Nodo = arista_aleatoria / 3;
    int x, y, z;
    int direccion = arista_aleatoria % 3;
    switch (direccion)
    {
    case 0:
        posiciones[0]=arista_aleatoria;
        posiciones[1]=arista_aleatoria+1;
        posiciones[2]=arista_aleatoria+3*ym[Nodo];
        posiciones[3]=arista_aleatoria+3*zm[Nodo]+1;
        break;

    case 1:
        posiciones[0]=arista_aleatoria-1;
        posiciones[1]=arista_aleatoria+1;
        posiciones[2]=arista_aleatoria+3*xm[Nodo]-1;
        posiciones[3]=arista_aleatoria+3*zm[Nodo]+1;
        break;
    case 2:
        posiciones[0]=arista_aleatoria;
        posiciones[1]=arista_aleatoria-1;
        posiciones[2]=arista_aleatoria+3*ym[Nodo];
        posiciones[3]=arista_aleatoria+3*xm[Nodo]-1;
        break;
    default:
        break;
    }
}

int indice_cociente_prob(int plaquetas[], int posiciones[]){
    int index=0;
    for(int i=0;i<4;i++){
        index=index+plaquetas[posiciones[i]];
    }
    return index/2+2;

}

void vector_cociente_prob(double *vector){
    vector[0]=exp(8*beta*J);
    vector[1]=exp(4*beta*J);
    vector[2]=1;
    vector[3]=exp(-4*beta*J);
    vector[4]=exp(-8*beta*J);
}

int un_paso_metropolis(int *aristas, int *plaquetas, int arista, double probabilidades[]){
    int posiciones[4];
    posicion_plaquetas(arista,posiciones);
    int index=indice_cociente_prob(plaquetas,posiciones);
    double r=fran();
    if(r<probabilidades[index]){
        for(int i=0;i<4;i++){
            plaquetas[posiciones[i]]=-plaquetas[posiciones[i]];
        }
        aristas[arista]=-aristas[arista];
        return 1;
    }
    return 0;
}

void N_pasos_metropolis(int N_sweeps, int *aristas, int *plaquetas, double probabilidades[], int *aceptadas){
    int V=3*L*L*L;
    for(int i=0;i<N_sweeps;i++){
        for(int j=0;j<V;j++){
        *aceptadas+=un_paso_metropolis(aristas,plaquetas,j,probabilidades);
    }
}
}


void calculo_promedios_wilson(int *s, int *plaquetas, int n, int m, int nodos_wilson[n][m][2], int n_pasos, int n_pasos_entre_mediciones, int n_termalizacion, double probabilidades[5]){
    int aceptadas = 0;

    // Reservamos memoria dinámica
    int **resultados_wilson = malloc(n * sizeof(int *));
    double **promedios_wilson = malloc(n * sizeof(double *));
    double **promedios_promedios_wilson = malloc(n * sizeof(double *));

    if (!resultados_wilson || !promedios_wilson || !promedios_promedios_wilson) {
        printf("Error reservando memoria\n");
        return;
    }
    for (int i = 0; i < n; i++) {
        resultados_wilson[i] = malloc(m * sizeof(int));
        promedios_wilson[i] = malloc(m * sizeof(double));
        promedios_promedios_wilson[i] = malloc(3 * sizeof(double));
        if (!resultados_wilson[i] || !promedios_wilson[i] || !promedios_promedios_wilson[i]) {
            printf("Error reservando memoria fila %d\n", i);
            // Liberamos lo que se haya reservado hasta ahora
            for (int j = 0; j <= i; j++) {
                free(resultados_wilson[j]);
                free(promedios_wilson[j]);
                free(promedios_promedios_wilson[j]);
            }
            free(resultados_wilson);
            free(promedios_wilson);
            free(promedios_promedios_wilson);
            return;
        }
    }

    // Inicializamos resultados a cero
    for(int i = 0; i < n; i++){        
        for(int j = 0; j < m; j++)
            resultados_wilson[i][j] = 0;
        for(int k=0;k<3;k++)
            promedios_promedios_wilson[i][k]=0.0;
    }

    // Realizamos la termalización
    N_pasos_metropolis(n_termalizacion, s, plaquetas, probabilidades, &aceptadas);

    for(int i = 0; i < n_pasos / n_pasos_entre_mediciones; i++){
        for(int j = 0; j < n; j++){
            for(int k = 0; k < m; k++){
                switch(nodos_wilson[j][k][1]){
                    case 0: // plano x
                        resultados_wilson[j][k] += un_loop_x(nodos_wilson[j][k][0], s, j+1);
                        break;
                    case 1: // plano y
                        resultados_wilson[j][k] += un_loop_y(nodos_wilson[j][k][0], s, j+1);
                        break;
                    case 2: // plano z
                        resultados_wilson[j][k] += un_loop_z(nodos_wilson[j][k][0], s, j+1);
                        break;
                }
            }
        }
        N_pasos_metropolis(n_pasos_entre_mediciones, s, plaquetas, probabilidades, &aceptadas);
    }

    char filename[256];
    const char* folder_promedios = "Resultados_simulacion/PROMEDIOS_WILSON";
    const char* folder_promedios_promedios = "Resultados_simulacion/PROMEDIOS_PROMEDIOS_WILSON";
    FILE* file_promedios,* file_promedios_promedios;
    int k = 0;

    while (1) {
        snprintf(filename, 256, "%s/I_%d.txt", folder_promedios, k);
        file_promedios = fopen(filename, "r");
        if (file_promedios) {
            fclose(file_promedios);
            k++;
        } else {
            break;
        }
    }

    file_promedios = fopen(filename, "w");
    if (!file_promedios) {
        printf("No se pudo crear el archivo %s\n", filename);
        // Liberamos memoria antes de salir
        for(int i = 0; i < n; i++){
            free(resultados_wilson[i]);
            free(promedios_wilson[i]);
            free(promedios_promedios_wilson[i]);
        }
        free(resultados_wilson);
        free(promedios_wilson);
        free(promedios_promedios_wilson);
        return;
    }

    snprintf(filename, 256, "%s/I_%d.txt", folder_promedios_promedios, k);
    file_promedios_promedios = fopen(filename, "w");
    if (!file_promedios_promedios) {
        printf("No se pudo crear el archivo %s\n", filename);
        // Liberamos memoria antes de salir
        for(int i = 0; i < n; i++){
            free(resultados_wilson[i]);
            free(promedios_wilson[i]);
            free(promedios_promedios_wilson[i]);
        }
        free(resultados_wilson);
        free(promedios_wilson);
        free(promedios_promedios_wilson);
        return;
    }


    for(int i = 0; i < n; i++){
        fprintf(file_promedios, "%d\t", i+1);
        for(int j = 0; j < m; j++){
            promedios_wilson[i][j] = (double)resultados_wilson[i][j] / (n_pasos / n_pasos_entre_mediciones);
            fprintf(file_promedios, "%f\t", promedios_wilson[i][j]);
        }
        fprintf(file_promedios, "\n");
    }

    fclose(file_promedios);


    for(int i=0;i<n;i++){
        double media=0.0;
        double desviacion;
        desviacion_estandar(m,promedios_wilson[i],&media,&desviacion);
        promedios_promedios_wilson[i][0]=i+1;
        promedios_promedios_wilson[i][1]=media;
        promedios_promedios_wilson[i][2]=desviacion;
    }


    for(int i=0;i<n;i++){
        fprintf(file_promedios_promedios, "%f\t%f\t%f\n", promedios_promedios_wilson[i][0], promedios_promedios_wilson[i][1], promedios_promedios_wilson[i][2]);
    }

    fclose(file_promedios_promedios);


    // Liberamos la memoria dinámica
    for(int i = 0; i < n; i++){
        free(resultados_wilson[i]);
        free(promedios_wilson[i]);
        free(promedios_promedios_wilson[i]);
    }
    free(resultados_wilson);
    free(promedios_wilson);
    free(promedios_promedios_wilson);
}



void dinamica_metropolis(
    int N_sweeps_entre_med,
    int N_medidas,
    double probabilidades[5],
    int *aristas,
    int *plaquetas
#ifdef correlacion
    , const char* filename_evolucion,
    const char* filename_param
#endif
) {
    int V = 3 * L * L * L;

    // Variables para medición de tiempos
    clock_t inicio_total, fin_total;
    clock_t inicio_io, fin_io;
    clock_t inicio_calculos, fin_calculos;
    clock_t inicio_metropolis, fin_metropolis;
    
    double tiempo_total, tiempo_io = 0.0, tiempo_calculos = 0.0, tiempo_metropolis = 0.0;

#ifndef correlacion
    char filename_param[256];
#endif

#ifndef correlacion
    // -------------------------
    // Determinar nombre del archivo de salida
    // -------------------------
    char* folder_inicial;
    char* folder_salida; 
    char* folder_final; 
    char* folder_param;

#ifdef termalizacion
    if (beta == 0.72) {
        folder_inicial = "Resultados_simulacion/TERMALIZACION/0.72/CONFIGURACION_INICIAL";
        folder_salida  = "Resultados_simulacion/TERMALIZACION/0.72/EVOLUCION";
        folder_final   = "Resultados_simulacion/TERMALIZACION/0.72/CONFIGURACION_FINAL";
        folder_param   = "Resultados_simulacion/TERMALIZACION/0.72/PARAMETROS";
    } else if (beta == 0.8) {
        folder_inicial = "Resultados_simulacion/TERMALIZACION/0.80/CONFIGURACION_INICIAL";
        folder_salida  = "Resultados_simulacion/TERMALIZACION/0.80/EVOLUCION";
        folder_final   = "Resultados_simulacion/TERMALIZACION/0.80/CONFIGURACION_FINAL";
        folder_param   = "Resultados_simulacion/TERMALIZACION/0.80/PARAMETROS";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO");
    }
#else 
    folder_inicial = "Resultados_simulacion/CONFIGURACION_INICIAL";
#endif
#endif // !correlacion
    
    // Iniciar medición de tiempo total
    inicio_total = clock();

#ifndef correlacion
    char filename[256];
    int d = 0;
    FILE* ftest;

    // Medir tiempo de I/O para búsqueda de archivos
    inicio_io = clock();
    
    // Buscar el mayor índice de configuración inicial existente
    while (1) {
        snprintf(filename, sizeof(filename), "%s/I_%d.txt", folder_inicial, d);
        ftest = fopen(filename, "r");
        if (ftest) {
            fclose(ftest);
            d++;
        } else break;
    }

    if (d == 0) {
        printf("No se encontraron archivos iniciales en %s\n", folder_inicial);
        return;
    }
    d--; // Tomamos el mayor k existente

    snprintf(filename, sizeof(filename), "%s/I_%d.txt", folder_salida, d);
    FILE* foutput = fopen(filename, "w");
#else
    // En modo correlación, usamos los nombres pasados como parámetros
    FILE* foutput = fopen(filename_evolucion, "w");
#endif

    if (!foutput) {
#ifdef correlacion
        printf("No se pudo crear el archivo %s\n", filename_evolucion);
#else
        printf("No se pudo crear el archivo %s\n", filename);
#endif
        return;
    }

#ifndef correlacion
    fin_io = clock();
    tiempo_io += (double)(fin_io - inicio_io) / CLOCKS_PER_SEC;
#endif

    // -------------------------
    // Dinámica de Metropolis
    // -------------------------
    int paso, aceptadas = 0;
    int wilsons_2[V], wilsons_3[V], wilsons_4[V], wilsons_5[V];
    int wilsons_6[V], wilsons_7[V], wilsons_8[V], wilsons_9[V], wilsons_10[V];
    double media_w2, media_w3, media_w4, media_w5, media_w6, media_w7, media_w8, media_w9, media_w10;
    double media_plaqueta, mag;

    for (paso = 1; paso <= N_medidas; paso++) {
        // Tiempo de Metropolis
        inicio_metropolis = clock();
        N_pasos_metropolis(N_sweeps_entre_med, aristas, plaquetas, probabilidades, &aceptadas);
        fin_metropolis = clock();
        tiempo_metropolis += (double)(fin_metropolis - inicio_metropolis) / CLOCKS_PER_SEC;

        // Tiempo de cálculos
        inicio_calculos = clock();
        
        // Calcular plaquetas
        media_plaqueta = promedio(plaquetas, V);

        // Calcular Wilson loops de distintos tamaños
        dame_wilsons_nn(aristas, wilsons_2, 2);
        dame_wilsons_nn(aristas, wilsons_3, 3);
        dame_wilsons_nn(aristas, wilsons_4, 4);
        dame_wilsons_nn(aristas, wilsons_5, 5);
        dame_wilsons_nn(aristas, wilsons_6, 6);
        dame_wilsons_nn(aristas, wilsons_7, 7);
        dame_wilsons_nn(aristas, wilsons_8, 8);
        dame_wilsons_nn(aristas, wilsons_9, 9);
        dame_wilsons_nn(aristas, wilsons_10, 10);

        media_w2  = promedio(wilsons_2, V);
        media_w3  = promedio(wilsons_3, V);
        media_w4  = promedio(wilsons_4, V);
        media_w5  = promedio(wilsons_5, V);
        media_w6  = promedio(wilsons_6, V);
        media_w7  = promedio(wilsons_7, V);
        media_w8  = promedio(wilsons_8, V);
        media_w9  = promedio(wilsons_9, V);
        media_w10 = promedio(wilsons_10, V);

        // Calcular magnetización
        mag = (double)magnetizacion(aristas) / V;

        fin_calculos = clock();
        tiempo_calculos += (double)(fin_calculos - inicio_calculos) / CLOCKS_PER_SEC;

        // -------------------------
        // Escritura (I/O)
        // -------------------------
        inicio_io = clock();

        fprintf(foutput, "%d\t%f\t", paso * N_sweeps_entre_med, media_plaqueta);
        fprintf(foutput, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t", 
                media_w2, media_w3, media_w4, media_w5, media_w6, 
                media_w7, media_w8, media_w9, media_w10);
        fprintf(foutput, "%f\n", mag);

        fin_io = clock();
        tiempo_io += (double)(fin_io - inicio_io) / CLOCKS_PER_SEC;
    }

    fclose(foutput);

#ifndef correlacion
    // -------------------------
    // Guardar configuración final (aristas) - I/O
    // -------------------------
    inicio_io = clock();
    
    char filename_final[256];
    snprintf(filename_final, sizeof(filename_final), "%s/I_%d.txt", folder_final, d);

    FILE* fconfig = fopen(filename_final, "w");
    if (!fconfig) {
        printf("No se pudo crear el archivo de configuración final %s\n", filename_final);
        return;
    }

    for (int i = 0; i < V; i++) {
        fprintf(fconfig, "%d\n", aristas[i]);
    }

    fclose(fconfig);
    fin_io = clock();
    tiempo_io += (double)(fin_io - inicio_io) / CLOCKS_PER_SEC;
#endif

    // -------------------------
    // Medición del tiempo total y mostrar resultados
    // -------------------------
    fin_total = clock();
    tiempo_total = (double)(fin_total - inicio_total) / CLOCKS_PER_SEC;

    printf("\n=== DESGLOSE DE TIEMPOS ===\n");
    printf("Tiempo total: %.2f s\n", tiempo_total);
    printf(" - Metropolis: %.2f s (%.1f%%)\n", tiempo_metropolis, (tiempo_metropolis/tiempo_total)*100.0);
    printf(" - Cálculos: %.2f s (%.1f%%)\n", tiempo_calculos, (tiempo_calculos/tiempo_total)*100.0);
    printf(" - I/O: %.2f s (%.1f%%)\n", tiempo_io, (tiempo_io/tiempo_total)*100.0);

    // -------------------------
    // Guardar tiempo total en archivo de parámetros
    // -------------------------
    inicio_io = clock();
#ifdef correlacion
    FILE* fparam = fopen(filename_param, "a");
#else
    snprintf(filename_param, sizeof(filename_param), "%s/I_%d.txt", folder_param, d);
    FILE* fparam = fopen(filename_param, "a");
#endif

    if (fparam) {
        fprintf(fparam, "Tiempo_total(s)\t%f\n", tiempo_total);
        fprintf(fparam," - Metropolis: %.2f s (%.1f%%)\n", tiempo_metropolis, (tiempo_metropolis/tiempo_total)*100.0);
        fprintf(fparam," - Cálculos: %.2f s (%.1f%%)\n", tiempo_calculos, (tiempo_calculos/tiempo_total)*100.0);
        fprintf(fparam," - I/O: %.2f s (%.1f%%)\n", tiempo_io, (tiempo_io/tiempo_total)*100.0);
        fclose(fparam);
#ifdef correlacion
        printf("Tiempo total guardado en %s\n", filename_param);
#endif
    } else {
        printf("No se pudo abrir el archivo de parámetros %s\n", filename_param);
    }
    
    fin_io = clock();
}


void estadistica(int *variable, double *media, double *desvest) {
    int N = 3 * L * L * L;
    double suma = 0.0;
    double suma2 = 0.0;

    for (int i = 0; i < N; i++) {
        double val = (double)variable[i];
        suma  += val;
        suma2 += val * val;
    }

    *media = suma / N;
    *desvest = sqrt( (suma2 / N) - (*media) * (*media) );
}

double promedio(int *variable, int N) {
    double suma = 0.0;
    for (int i = 0; i < N; i++)
        suma += (double)variable[i];
    return suma / N;
}




void guarda_parametros(int n, int m, int n_pasos, int n_pasos_entre_mediciones, int n_termalizacion){

    const char* folder = "PARAMETROS";
    char *filename = (char *)malloc(256 * sizeof(char));

    FILE* file;
    int k = 0;


    // Buscamos el primer V_k.txt que no exista
    while (1) {
        snprintf(filename, 256, "%s/I_%d.txt", folder, k);
        file = fopen(filename, "r");
        if (file) {
            // El archivo existe, cerramos y probamos el siguiente
            fclose(file);
            k++;
        } else {
            // No existe, podemos usar este k
            break;
        }
    }


    // Creamos el archivo para escritura
    file = fopen(filename, "w");
    if (!file) {
        printf("No se pudo crear el archivo %s\n", filename);
        filename[0] = '\0'; // indicamos fallo
        return;
    }


    // Escribimos los parámetros
    fprintf(file, "L %d\n", L);
    fprintf(file, "beta %g\n", beta);
    fprintf(file, "J %g\n", J);
    fprintf(file, "n %d\n", n);
    fprintf(file, "m %d\n", m);
    fprintf(file, "n_pasos %d\n", n_pasos);
    fprintf(file, "n_pasos_entre_mediciones %d\n", n_pasos_entre_mediciones);
    fprintf(file, "n_termalizacion %d\n", n_termalizacion);



    fclose(file);


    printf("Archivo creado: %s\n", filename);


}


// -------------------------------------------------------------
// Función auxiliar: calcula media y desviación estándar de la media
// -------------------------------------------------------------
void calcular_media_std(double *data, int N, double *media, double *std) {
    double sum = 0.0, sum2 = 0.0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
        sum2 += data[i] * data[i];
    }
    *media = sum / N;
    double var = (sum2 / N) - (*media) * (*media);
    *std = sqrt(fabs(var) / N);
}

// -------------------------------------------------------------
// Función: crea ficheros ventana I_k.txt en carpeta VENTANAS/EVOLUCION
// -------------------------------------------------------------
void crear_ventanas(int k_ini, int k_final, int N_ventana, int N_SALTO) {
    for (int k = k_ini; k <= k_final; k++) {
        char input_path[512], output_path[512];
        sprintf(input_path, "Resultados_simulacion/TERMALIZACION/0.72/EVOLUCION/I_%d.txt", k);
        sprintf(output_path, "Resultados_simulacion/TERMALIZACION/0.72/VENTANAS/EVOLUCION/I_%d.txt", k);

        FILE *fin = fopen(input_path, "r");
        if (!fin) { printf("No se pudo abrir %s\n", input_path); continue; }

        // Contamos líneas
        int n = 0;
        char buf[2048];
        while (fgets(buf, sizeof(buf), fin)) n++;
        rewind(fin);

        // Reservamos memoria
        double *tiempo = malloc(n * sizeof(double));
        double **obs = malloc(n * sizeof(double*));
        for (int i = 0; i < n; i++)
            obs[i] = malloc(11 * sizeof(double));

        // Leemos todo el archivo
        int idx = 0;
        while (1) {
            double t, tmp[11];
            int read = fscanf(fin, "%lf", &t);
            for (int j = 0; j < 11; j++) read += fscanf(fin, "%lf", &tmp[j]);
            if (read != 12) break;
            tiempo[idx] = t;
            for (int j = 0; j < 11; j++) obs[idx][j] = tmp[j];
            idx++;
        }
        fclose(fin);

        FILE *fout = fopen(output_path, "w");
        if (!fout) { printf("No se pudo crear %s\n", output_path); goto cleanup; }

        // Cabecera
        fprintf(fout, "# tiempo");
        for (int j = 0; j < 11; j++)
            fprintf(fout, "\tmean_var%d\tstd_var%d", j + 1, j + 1);
        fprintf(fout, "\n");

        // Ventanas deslizantes con salto N_SALTO
        for (int i = 0; i + N_ventana * N_SALTO <= idx; i++) {
            fprintf(fout, "%.0f", tiempo[i + N_ventana * N_SALTO - 1]);
            for (int j = 0; j < 11; j++) {
                double *datos_ventana = malloc(N_ventana * sizeof(double));
                for (int w = 0; w < N_ventana; w++)
                    datos_ventana[w] = obs[i + w * N_SALTO][j];
                double m, s;
                calcular_media_std(datos_ventana, N_ventana, &m, &s);
                fprintf(fout, "\t%lf\t%lf", m, s);
                free(datos_ventana);
            }
            fprintf(fout, "\n");
        }

        fclose(fout);

    cleanup:
        for (int i = 0; i < n; i++) free(obs[i]);
        free(obs);
        free(tiempo);

        printf("Ventanas creadas: %s\n", output_path);
    }
}

// -------------------------------------------------------------
// Función: calcula media y desviación estándar global (entre configuraciones)
// -------------------------------------------------------------
void crear_media_global(int k_ini, int k_final) {
    int num_var = 11; // número de variables

    char output_path[512];
    sprintf(output_path, "Resultados_simulacion/TERMALIZACION/0.72/VENTANAS/EVOLUCION/MEDIA_%d_%d.txt", k_ini, k_final);

    FILE *fout = fopen(output_path, "w");
    if (!fout) {
        printf("No se pudo crear %s\n", output_path);
        return;
    }

    // Leemos el primer archivo para determinar n y tiempos
    char first_path[512];
    sprintf(first_path, "Resultados_simulacion/TERMALIZACION/0.72/VENTANAS/EVOLUCION/I_%d.txt", k_ini);
    FILE *f0 = fopen(first_path, "r");
    if (!f0) {
        printf("No se pudo abrir %s\n", first_path);
        fclose(fout);
        return;
    }

    // Contamos líneas de datos (ignorando cabecera)
    int n = 0;
    char buffer[2048];
    while (fgets(buffer, sizeof(buffer), f0)) {
        if (buffer[0] != '#') n++;
    }
    rewind(f0);

    double *tiempo = malloc(n * sizeof(double));
    // Reservamos memoria para todas las configuraciones y variables
    int num_conf = k_final - k_ini + 1;
    double ***medias = malloc(num_conf * sizeof(double **));
    double ***desvs  = malloc(num_conf * sizeof(double **));
    for (int c = 0; c < num_conf; c++) {
        medias[c] = malloc(n * sizeof(double *));
        desvs[c]  = malloc(n * sizeof(double *));
        for (int i = 0; i < n; i++) {
            medias[c][i] = malloc(num_var * sizeof(double));
            desvs[c][i]  = malloc(num_var * sizeof(double));
        }
    }

    // Leemos cada archivo de configuración
    for (int k = k_ini; k <= k_final; k++) {
        char path[512];
        sprintf(path, "Resultados_simulacion/TERMALIZACION/0.72/VENTANAS/EVOLUCION/I_%d.txt", k);
        FILE *f = fopen(path, "r");
        if (!f) continue;

        int i = 0;
        while (fgets(buffer, sizeof(buffer), f)) {
            if (buffer[0] == '#') continue;

            // Leemos tiempo
            char *ptr = buffer;
            sscanf(ptr, "%lf", &tiempo[i]);

            // Avanzamos al siguiente número
            for (int j = 0; j < 2*num_var; j++) {
                double val;
                sscanf(ptr, "%lf", &val);
                ptr = strchr(ptr, '\t'); // asumimos tab como separador
                if (!ptr) break;
                ptr++; // saltar tab
            }

            // Leemos media y desviación de cada variable
            ptr = buffer;
            double tmp;
            sscanf(ptr, "%lf", &tmp); // saltamos tiempo
            ptr = strchr(ptr, '\t');
            if (ptr) ptr++;
            for (int v = 0; v < num_var; v++) {
                double m,s;
                sscanf(ptr, "%lf", &m);
                ptr = strchr(ptr, '\t'); if (!ptr) break; ptr++;
                sscanf(ptr, "%lf", &s);
                ptr = strchr(ptr, '\t'); if (!ptr) break; ptr++;
                medias[k-k_ini][i][v] = m;
                desvs[k-k_ini][i][v]  = s;
            }
            i++;
        }
        fclose(f);
    }

    // Calculamos media global y desviación entre configuraciones
    fprintf(fout, "# tiempo");
    for (int v = 0; v < num_var; v++)
        fprintf(fout, "\tmean_var%d\tstd_var%d", v+1, v+1);
    fprintf(fout, "\n");

    for (int i = 0; i < n; i++) {
        fprintf(fout, "%.0f", tiempo[i]); // tiempo real del archivo
        for (int v = 0; v < num_var; v++) {
            double sum = 0.0;
            for (int c = 0; c < num_conf; c++)
                sum += medias[c][i][v];
            double mean = sum / num_conf;

            double var = 0.0;
            for (int c = 0; c < num_conf; c++) {
                double diff = medias[c][i][v] - mean;
                var += diff * diff;
            }
            double std = sqrt(var / num_conf);
            fprintf(fout, "\t%lf\t%lf", mean, std);
        }
        fprintf(fout, "\n");
    }

    fclose(fout);
// ----------------------------------------------------
// Generación archivo de compatibilidad global
// ----------------------------------------------------
char compat_path[512];
sprintf(compat_path, "Resultados_simulacion/TERMALIZACION/0.72/VENTANAS/COMPATIBILIDAD_%d_%d.txt", k_ini, k_final);
FILE *fc = fopen(compat_path, "w");
if (!fc) {
    printf("No se pudo crear %s\n", compat_path);
    return;
}

fprintf(fc, "=========== ANALISIS DE COMPATIBILIDAD GLOBAL (como mucho 1 variable fuera) ===========\n\n");

for (int k = k_ini; k <= k_final; k++) {
    fprintf(fc, "I_%d.txt Compatible con la media en los tramos:\n\n", k);
    int en_intervalo = 0;
    double inicio = 0.0;

    for (int i = 0; i < n; i++) {
        int num_incomp = 0;

        for (int v = 0; v < num_var; v++) {
            double M = 0.0, Sm = 0.0;
            for (int c = 0; c < num_conf; c++)
                M += medias[c][i][v];
            M /= num_conf;
            for (int c = 0; c < num_conf; c++)
                Sm += (medias[c][i][v]-M)*(medias[c][i][v]-M);
            Sm = sqrt(Sm/num_conf);

            double sigma = sqrt(desvs[k-k_ini][i][v]*desvs[k-k_ini][i][v] + Sm*Sm);
            double diff = M - medias[k-k_ini][i][v];

            if (fabs(diff) > 2.0*sigma) num_incomp++;
        }

        // Intervalo global compatible si como mucho 1 variable está fuera
        if (num_incomp <= 1) {
            if (!en_intervalo) { inicio = tiempo[i]; en_intervalo = 1; }
        } else if (en_intervalo) {
            fprintf(fc, "[%.0f, %.0f]\n", inicio, tiempo[i-1]);
            en_intervalo = 0;
        }
    }

    if (en_intervalo) fprintf(fc, "[%.0f, %.0f]\n", inicio, tiempo[n-1]);
    fprintf(fc, "\n");
}

fclose(fc);
}
















void dinamica_metropolis_main(
    int N_sweeps_entre_med,
    int N_medidas,
    double probabilidades[5],
    int *aristas,
    int *plaquetas, char *filename_aristas
) {
    int V = 3 * L * L * L;

    // Variables para medición de tiempos
    clock_t inicio_total, fin_total;
    clock_t inicio_io, fin_io;
    clock_t inicio_calculos, fin_calculos;
    clock_t inicio_metropolis, fin_metropolis;
    
    double tiempo_total, tiempo_io = 0.0, tiempo_calculos = 0.0, tiempo_metropolis = 0.0;

    // -------------------------
    // Determinar nombre del archivo de salida
    // -------------------------
    char* folder_inicial;
    char* folder_salida; 
    char* folder_final; 
    char* folder_param;

#ifdef Na
    if (beta == 0.72) {
        folder_salida  = "Resultados_simulacion/MAIN/0.72/EVOLUCION/NASIO";
        folder_param   = "Resultados_simulacion/MAIN/0.72/PARAMETROS/NASIO";
    } else if (beta == 0.8) {
        folder_salida  = "Resultados_simulacion/MAIN/0.8/EVOLUCION/NASIO";
        folder_param   = "Resultados_simulacion/MAIN/0.8/PARAMETROS/NASIO";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Ba)
    if (beta == 0.72) {
        folder_salida  = "Resultados_simulacion/MAIN/0.72/EVOLUCION/BARRACHINA";
        folder_param   = "Resultados_simulacion/MAIN/0.72/PARAMETROS/BARRACHINA";
    } else if (beta == 0.8) {
        folder_salida  = "Resultados_simulacion/MAIN/0.8/EVOLUCION/BARRACHINA";
        folder_param   = "Resultados_simulacion/MAIN/0.8/PARAMETROS/BARRACHINA";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Jo)
    if (beta == 0.72) {
        folder_salida  = "Resultados_simulacion/MAIN/0.72/EVOLUCION/JOEL";
        folder_param   = "Resultados_simulacion/MAIN/0.72/PARAMETROS/JOEL";
    } else if (beta == 0.8) {
        folder_salida  = "Resultados_simulacion/MAIN/0.8/EVOLUCION/JOEL";
        folder_param   = "Resultados_simulacion/MAIN/0.8/PARAMETROS/JOEL";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#else
    #error "Debes definir Na, Ba o Jo antes de compilar"
#endif

    // Iniciar medición de tiempo total
    inicio_total = clock();

    char filename[256];
    int d = 0;
    FILE* ftest;

    // Medir tiempo de I/O para búsqueda de archivos
    inicio_io = clock();
    
    // Buscar el mayor índice de las evoluciones existente
    while (1) {
        snprintf(filename, sizeof(filename), "%s/I_%d.txt", folder_salida, d);
        ftest = fopen(filename, "r");
        if (ftest) {
            fclose(ftest);
            d++;
        } else break;
    }

    if (d < 0) {
        printf("No se encontraron archivos iniciales en %s\n", folder_inicial);
        return;
    }
    d; // Tomamos el mayor k existente

    snprintf(filename, sizeof(filename), "%s/I_%d.txt", folder_salida, d);
    FILE* foutput = fopen(filename, "w");

    if (!foutput) {
        printf("No se pudo crear el archivo %s\n", filename);
        return;
    }

    fin_io = clock();
    tiempo_io += (double)(fin_io - inicio_io) / CLOCKS_PER_SEC;

    // -------------------------
    // Dinámica de Metropolis
    // -------------------------
    int paso, aceptadas = 0;
    int wilsons_2[V], wilsons_3[V], wilsons_4[V], wilsons_5[V];
    int wilsons_6[V], wilsons_7[V], wilsons_8[V], wilsons_9[V], wilsons_10[V];
    double media_w2, media_w3, media_w4, media_w5, media_w6, media_w7, media_w8, media_w9, media_w10;
    double media_plaqueta, mag;

    for (paso = 1; paso <= N_medidas; paso++) {
        // Tiempo de Metropolis
        inicio_metropolis = clock();
        N_pasos_metropolis(N_sweeps_entre_med, aristas, plaquetas, probabilidades, &aceptadas);
        fin_metropolis = clock();
        tiempo_metropolis += (double)(fin_metropolis - inicio_metropolis) / CLOCKS_PER_SEC;

        // Tiempo de cálculos
        inicio_calculos = clock();
        
        // Calcular plaquetas
        media_plaqueta = promedio(plaquetas, V);

        // Calcular Wilson loops de distintos tamaños
        dame_wilsons_nn(aristas, wilsons_2, 2);
        dame_wilsons_nn(aristas, wilsons_3, 3);
        dame_wilsons_nn(aristas, wilsons_4, 4);
        dame_wilsons_nn(aristas, wilsons_5, 5);
        dame_wilsons_nn(aristas, wilsons_6, 6);
        dame_wilsons_nn(aristas, wilsons_7, 7);
        dame_wilsons_nn(aristas, wilsons_8, 8);
        dame_wilsons_nn(aristas, wilsons_9, 9);
        dame_wilsons_nn(aristas, wilsons_10, 10);

        media_w2  = promedio(wilsons_2, V);
        media_w3  = promedio(wilsons_3, V);
        media_w4  = promedio(wilsons_4, V);
        media_w5  = promedio(wilsons_5, V);
        media_w6  = promedio(wilsons_6, V);
        media_w7  = promedio(wilsons_7, V);
        media_w8  = promedio(wilsons_8, V);
        media_w9  = promedio(wilsons_9, V);
        media_w10 = promedio(wilsons_10, V);

        // Calcular magnetización
        mag = (double)magnetizacion(aristas) / V;

        fin_calculos = clock();
        tiempo_calculos += (double)(fin_calculos - inicio_calculos) / CLOCKS_PER_SEC;

        // -------------------------
        // Escritura (I/O)
        // -------------------------
        inicio_io = clock();

        fprintf(foutput, "%d\t%f\t", paso * N_sweeps_entre_med, media_plaqueta);
        fprintf(foutput, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t", 
                media_w2, media_w3, media_w4, media_w5, media_w6, 
                media_w7, media_w8, media_w9, media_w10);
        fprintf(foutput, "%f\n", mag);

        fin_io = clock();
        tiempo_io += (double)(fin_io - inicio_io) / CLOCKS_PER_SEC;
    }

    fclose(foutput);

    // -------------------------
    // Guardar configuración final (aristas) - I/O
    // -------------------------
    inicio_io = clock();


    FILE* fconfig = fopen(filename_aristas, "w");
    if (!fconfig) {
        printf("No se pudo crear el archivo de configuración final %s\n", filename_aristas);
        return;
    }

    for (int i = 0; i < V; i++) {
        fprintf(fconfig, "%d\n", aristas[i]);
    }

    fclose(fconfig);
    fin_io = clock();
    tiempo_io += (double)(fin_io - inicio_io) / CLOCKS_PER_SEC;

    // -------------------------
    // Medición del tiempo total y mostrar resultados
    // -------------------------
    fin_total = clock();
    tiempo_total = (double)(fin_total - inicio_total) / CLOCKS_PER_SEC;

    printf("\n=== DESGLOSE DE TIEMPOS ===\n");
    printf("Tiempo total: %.2f s\n", tiempo_total);
    printf(" - Metropolis: %.2f s (%.1f%%)\n", tiempo_metropolis, (tiempo_metropolis/tiempo_total)*100.0);
    printf(" - Cálculos: %.2f s (%.1f%%)\n", tiempo_calculos, (tiempo_calculos/tiempo_total)*100.0);
    printf(" - I/O: %.2f s (%.1f%%)\n", tiempo_io, (tiempo_io/tiempo_total)*100.0);

    // -------------------------
    // Guardar tiempo total en archivo de parámetros
    // -------------------------
    inicio_io = clock();

    int d2=0;
    char filename_param[256];
        FILE* ftest2;


    while (1) {
        snprintf(filename_param, sizeof(filename), "%s/I_%d.txt", folder_param, d2);
        ftest2 = fopen(filename_param, "r");
        if (ftest2) {
            fclose(ftest2);
            d2++;
        } else break;
    }
    d2--;


    snprintf(filename_param, sizeof(filename_param), "%s/I_%d.txt", folder_param, d2);
    FILE* fparam = fopen(filename_param, "a");

    if (fparam) {
        fprintf(fparam, "Tiempo_total(s)\t%f\n", tiempo_total);
        fprintf(fparam," - Metropolis: %.2f s (%.1f%%)\n", tiempo_metropolis, (tiempo_metropolis/tiempo_total)*100.0);
        fprintf(fparam," - Cálculos: %.2f s (%.1f%%)\n", tiempo_calculos, (tiempo_calculos/tiempo_total)*100.0);
        fprintf(fparam," - I/O: %.2f s (%.1f%%)\n", tiempo_io, (tiempo_io/tiempo_total)*100.0);
        fclose(fparam);
    } else {
        printf("No se pudo abrir el archivo de parámetros %s\n", filename_param);
    }
    
    fin_io = clock();
}



