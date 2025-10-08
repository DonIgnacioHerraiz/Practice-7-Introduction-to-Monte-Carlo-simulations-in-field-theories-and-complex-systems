#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"

/*
PARA EJECUTAR ESTE TEST HAY QUE PONER BETA=0 EN EL ARCHIVO parametros.h 
*/

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

int main(){
    inicializa_PR(12345);
    int s[3*L*L*L], plaquetas[3*L*L*L];
    inicializa_vectores_de_vecinos();

    FILE* fout;
    fout=fopen("TESTS/temperatura_cero.txt","w");
    if(fout==NULL){
        printf("No se pudo crear el archivo temperatura_infinita.txt\n");
        return 1;
    }

    int n_pruebas=50000;
    double energia1=0,energia2=0;
 
    double probabilidades[5];
    vector_cociente_prob(probabilidades);
    crea_configuracionInicial(0, s);
    dame_plaquetas(s, plaquetas);

    energia1=energia_normalizada(plaquetas);

    int aceptadas=0;
    int counter=0;

    fprintf(fout,"%f\n",energia1);   

    for(int i=0;i<n_pruebas;i++){
        N_pasos_metropolis(1, s, plaquetas, probabilidades, &aceptadas);
        energia2=energia_normalizada(plaquetas);

        if((energia1-energia2)<0){
            printf("-------La energia no es estrictamente decreciente------");
            printf("La dif: %f",energia1-energia2);
            printf("\nEnergia antes del cambio: %f", energia1);
            printf("\nEnergia despues del cambio: %f", energia2);
            counter++;
        }else if((energia1-energia2)!=4.0 && (energia1-energia2)!=8.0 && (energia1-energia2)!=0.0){
            printf("La dif: %f",energia1-energia2);
        }
        energia1=energia2;
        fprintf(fout,"%f\n",energia1);
    }

    printf("\nNumero de aceptadas: %d", aceptadas);
    printf("\nNumero de pruebas: %d", n_pruebas);
    printf("\nNumero de veces que la energia no aumento: %d", counter);
    printf("\nPorcentaje de aceptacion: %f %%", (double)aceptadas/n_pruebas*100);


    fclose(fout);
    return 0;
}