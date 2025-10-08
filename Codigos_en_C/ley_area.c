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
    double probabilidades[5];
    int aceptadas=0;
    vector_cociente_prob(probabilidades);
    inicializa_vectores_de_vecinos();
    int n=10;
    int m=20000;
    int n_pasos=500000;
    int n_pasos_entre_mediciones=1000;
    int n_termalizacion=100000;
    int nodos_wilson[n][m][2];

    guarda_parametros(n, m, n_pasos, n_pasos_entre_mediciones, n_termalizacion);


    inicializa_nodos_wilson(n,m,nodos_wilson);

  
    crea_configuracionInicial(0, s);

    dame_plaquetas(s, plaquetas);


    calculo_promedios_wilson(s, plaquetas, n, m, nodos_wilson, n_pasos, n_pasos_entre_mediciones, n_termalizacion, probabilidades);

    return 0;
}