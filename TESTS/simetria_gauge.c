#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"

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

    int n_pruebas=50000;

    double energia1=0,energia2=0;
 

    crea_configuracionInicial(0, s);

    dame_plaquetas(s, plaquetas);

    energia1=energia_normalizada(plaquetas);

    for(int i=0;i<n_pruebas;i++){
        cambio_aristas_nodo(s);
        dame_plaquetas(s, plaquetas);
        energia2=energia_normalizada(plaquetas);

        if(fabs(energia1-energia2)>1e-06){
            printf("-------Violacion de la simetria Ising-Gauge------");
            printf("\nEnergia antes del cambio: %f", energia1);
            printf("\nEnergia despues del cambio: %f", energia2);
        }
        energia1=energia2;
    }





    return 0;
}