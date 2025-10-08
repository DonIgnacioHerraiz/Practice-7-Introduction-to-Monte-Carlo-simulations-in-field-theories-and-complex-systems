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

    const char* folder = "Resultados_simulacion/CONFIGURACION_INICIAL";
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
    coordenadas_nodo(Nodo, &x, &y, &z);
    int direccion = arista_aleatoria % 3;
    switch (direccion)
    {
    case 0:
        posiciones[0]=arista_aleatoria;
        posiciones[1]=arista_aleatoria+1;
        posiciones[2]=arista_aleatoria+3*ym[y];
        posiciones[3]=arista_aleatoria+3*zm[z]+1;
        break;

    case 1:
        posiciones[0]=arista_aleatoria-1;
        posiciones[1]=arista_aleatoria+1;
        posiciones[2]=arista_aleatoria+3*xm[x]-1;
        posiciones[3]=arista_aleatoria+3*zm[z]+1;
        break;
    case 2:
        posiciones[0]=arista_aleatoria;
        posiciones[1]=arista_aleatoria-1;
        posiciones[2]=arista_aleatoria+3*ym[y];
        posiciones[3]=arista_aleatoria+3*xm[x]-1;
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
    vector[0]=exp(-8*beta*J);
    vector[1]=exp(-4*beta*J);
    vector[2]=1;
    vector[3]=exp(4*beta*J);
    vector[4]=exp(8*beta*J);
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

void N_pasos_metropolis(int N, int *aristas, int *plaquetas, double probabilidades[], int *aceptadas){
    for(int i=0;i<N;i++){
        int arista=arista_aleatoria();
        *aceptadas+=un_paso_metropolis(aristas,plaquetas,arista,probabilidades);
    }
}


void calculo_promedios_wilson(int *s, int *plaquetas, int n, int m, int nodos_wilson[n][m][2], int n_pasos, int n_pasos_entre_mediciones, int n_termalizacion, double probabilidades[5]){
    int aceptadas = 0;

    // Reservamos memoria dinámica
    int **resultados_wilson = malloc(n * sizeof(int *));
    double **promedios_wilson = malloc(n * sizeof(double *));
    if (!resultados_wilson || !promedios_wilson) {
        printf("Error reservando memoria\n");
        return;
    }
    for (int i = 0; i < n; i++) {
        resultados_wilson[i] = malloc(m * sizeof(int));
        promedios_wilson[i] = malloc(m * sizeof(double));
        if (!resultados_wilson[i] || !promedios_wilson[i]) {
            printf("Error reservando memoria fila %d\n", i);
            // Liberamos lo que se haya reservado hasta ahora
            for (int j = 0; j <= i; j++) {
                free(resultados_wilson[j]);
                free(promedios_wilson[j]);
            }
            free(resultados_wilson);
            free(promedios_wilson);
            return;
        }
    }

    // Inicializamos resultados a cero
    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++)
            resultados_wilson[i][j] = 0;

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
    const char* folder = "Resultados_simulacion/PROMEDIOS_WILSON";
    FILE* file;
    int k = 0;

    while (1) {
        snprintf(filename, 256, "%s/I_%d.txt", folder, k);
        file = fopen(filename, "r");
        if (file) {
            fclose(file);
            k++;
        } else {
            break;
        }
    }

    file = fopen(filename, "w");
    if (!file) {
        printf("No se pudo crear el archivo %s\n", filename);
        // Liberamos memoria antes de salir
        for(int i = 0; i < n; i++){
            free(resultados_wilson[i]);
            free(promedios_wilson[i]);
        }
        free(resultados_wilson);
        free(promedios_wilson);
        return;
    }

    for(int i = 0; i < n; i++){
        fprintf(file, "%d\t", i+1);
        for(int j = 0; j < m; j++){
            promedios_wilson[i][j] = (double)resultados_wilson[i][j] / (n_pasos / n_pasos_entre_mediciones);
            fprintf(file, "%f\t", promedios_wilson[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);

    // Liberamos la memoria dinámica
    for(int i = 0; i < n; i++){
        free(resultados_wilson[i]);
        free(promedios_wilson[i]);
    }
    free(resultados_wilson);
    free(promedios_wilson);
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