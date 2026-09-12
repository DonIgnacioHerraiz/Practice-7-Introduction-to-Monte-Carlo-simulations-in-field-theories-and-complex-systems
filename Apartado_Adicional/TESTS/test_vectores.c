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

    int n=24;

    printf("\n Vector xp[%d]=%d",n,xp[n]);
    printf("\n Vector yp[%d]=%d",n,yp[n]);
    printf("\n Vector zp[%d]=%d",n,zp[n]);
    printf("\n Vector xm[%d]=%d",n,xm[n]);
    printf("\n Vector ym[%d]=%d",n,ym[n]);
    printf("\n Vector zm[%d]=%d",n,zm[n]);



    return 0;
}