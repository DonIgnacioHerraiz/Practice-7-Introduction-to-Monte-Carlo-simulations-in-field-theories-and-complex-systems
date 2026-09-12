#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

int plaquetas_identicas(int *plaquetas_1, int *plaquetas_2){

    for(int i=0;i<3*L*L*L;i++){
        if(plaquetas_1[i]!=plaquetas_2[i]) return 0;
    }
    return 1;
}

void iguala_aristas(int *s1, int *s2){
        for(int i=0;i<3*L*L*L;i++){
        s2[i]=s1[i];
    }

}

int main(){
    int FLAG=3; //1 PARA PLAQUETAS, 2 PARA WILSONS GAUGE, 3 PARA VER WILSONS EXPLICITOS
    if(FLAG==1){//COMPRUEBA QUE EL VALOR DE LAS PLAQUETAS TIENE SIMETRÍA GAUGE
    inicializa_PR(12345);
    int s[3*L*L*L], plaquetas_inicial[3*L*L*L],plaquetas_cambio[3*L*L*L];
    inicializa_vectores_de_vecinos();

    int n_pruebas=50000;

    crea_configuracionInicial(0, s);

    dame_plaquetas(s, plaquetas_inicial);
    int n_pasos=0;

    for(int i=0;i<n_pruebas;i++){
        cambio_aristas_nodo(s);
        dame_plaquetas(s, plaquetas_cambio);

        if(plaquetas_identicas(plaquetas_inicial,plaquetas_cambio)==0){
            printf("-------Violacion de la simetria Ising-Gauge------");
        }else{
            n_pasos++;
        }
    }
    printf("%d",n_pasos);
    }else{if(FLAG==2){//COMPRUEBA QUE LOS WILSONS TIENEN SIMETRÍA GAUGE
                      //COGE LOS LOOPS DEL NODO DEL CUAL SE CAMBIAN LAS ARISTAS


    inicializa_PR(12345);
    int s_inicial[3*L*L*L],s_cambio[3*L*L*L];
    inicializa_vectores_de_vecinos();

    int n_pruebas=50000;

    crea_configuracionInicial(0, s_inicial);
    iguala_aristas(s_inicial, s_cambio);

    int n_pasos=0;
    int nodo;
    int n=4;

    for(int i=0;i<n_pruebas;i++){
        nodo=cambio_aristas_nodo(s_cambio);
        int wilson_y_inicial=un_loop_y(nodo, s_inicial, n);
        int wilson_y_cambio=un_loop_y(nodo, s_cambio, n);

        if(wilson_y_inicial!=wilson_y_cambio){
            printf("-------Violacion de la simetria Ising-Gauge------");
        }else{
            n_pasos++;
        }
    }
                printf("%d",n_pasos);
    }else{
        if(FLAG==3){

            inicializa_PR(12345);
            int s_inicial[3*L*L*L],s_cambio[3*L*L*L];
            inicializa_vectores_de_vecinos();
            crea_configuracionInicial(0, s_inicial);

            for(int i=0;i<L*L*L;i++){
                printf("Nodo: %d\t Loop: %d \n",i+1,un_loop_z(i,s_inicial,1));

        
            }

        }
    }


}



    return 0;
}