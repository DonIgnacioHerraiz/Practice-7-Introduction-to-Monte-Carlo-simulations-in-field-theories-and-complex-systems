#include "funciones_dinamica.h"

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