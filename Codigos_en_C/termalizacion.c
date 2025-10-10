#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

void guardar_parametros(int FLAG_INI, int N_sweps_entre_medidas,int N_medidas) {
    char* folder_param;
    if(beta==0.72){
        folder_param = "Resultados_simulacion/TERMALIZACION/0.72/PARAMETROS";
    }else{
        if(beta==0.80){
            folder_param = "Resultados_simulacion/TERMALIZACION/0.80/PARAMETROS";
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
    if(FLAG_INI==1){fprintf(fparam, "estado_inicial\tcold\n");
    }else{
        fprintf(fparam, "estado_inicial\thot\n");
    }
    fprintf(fparam, "N_sweps_entre_medidas\t%d\n", N_sweps_entre_medidas);
    fprintf(fparam, "N_medidas\t%d\n", N_medidas);

    fclose(fparam);
    printf("Archivo de parámetros creado: %s\n", filename);
}

int main(){
    
    inicializa_PR(12345);
    int s[3*L*L*L], plaquetas[3*L*L*L];
    double probabilidades[5];
    int FLAG_INI=1;
    int un_sweep=3*L*L*L;
    int N_sweps_entre_med=1;
    int N_medidas=40000;

    for(int FLAG_INI=0;FLAG_INI<2;FLAG_INI++){

    for(int j=0;j<10;j++){
    vector_cociente_prob(probabilidades);
    inicializa_vectores_de_vecinos();
    crea_configuracionInicial(FLAG_INI, s);
    dame_plaquetas(s, plaquetas);
    guardar_parametros(FLAG_INI,N_sweps_entre_med,N_medidas);
    dinamica_metropolis(N_sweps_entre_med, N_medidas, probabilidades,s, plaquetas);
    }
    }

    int k_ini = 0, k_final = 19;
    int N_ventana = 5;
    int N_salto=3*35;

    crear_ventanas(k_ini, k_final, N_ventana,N_salto);
    crear_media_global(k_ini, k_final);



    return 0;
}