#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

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

int main(){

    inicializa_PR((int)time(NULL));
    int s[3*L*L*L], plaquetas[3*L*L*L];
    double probabilidades[5];
    int N_sweps_entre_med=35*3;
    int N_medidas=50;
    char input_file[512]; 


    for(int j=0;j<10;j++){
    int k_ini=(int)(fran()*20);
    sprintf(input_file, "Resultados_simulacion/MAIN/0.72/CONFIGURACIONES/I_%d.txt", k_ini);
    vector_cociente_prob(probabilidades);
    inicializa_vectores_de_vecinos();
    lee_configuracionInicial(s, input_file);
    dame_plaquetas(s, plaquetas);
    guardar_parametros(k_ini,N_sweps_entre_med,N_medidas);
    dinamica_metropolis_main(N_sweps_entre_med, N_medidas, probabilidades,s, plaquetas,input_file);
    }

    return 0;
}