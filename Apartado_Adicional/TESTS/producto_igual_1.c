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
    int s[3*L*L*L],wilsons[3*L*L*L];
    int n_wilsons=3*L*L*L;
    inicializa_vectores_de_vecinos();

    int tamaño_maximo_wilson=30;

    crea_configuracion(0, s);

    int producto_wilsons=1;
    


    for(int n=1;n<tamaño_maximo_wilson;n++){
        dame_wilsons_nn(s,wilsons,n);
        producto_wilsons=1;
        for(int i=0;i<n_wilsons;i++){
            producto_wilsons*=wilsons[i];
        }
        if(producto_wilsons!=1){
            printf("-------Error en la prueba------");
            printf("\nPara el tamaño %d el prodcuto de wilsons da %d", n,producto_wilsons);
        }
    }





    return 0;
}