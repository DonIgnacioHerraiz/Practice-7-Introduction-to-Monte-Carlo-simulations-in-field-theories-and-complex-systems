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
    int s[3*L*L*L];
    crea_configuracionInicial(0, s);
    inicializa_vectores_de_vecinos();
    int n=5;

    printf("%d\t%d\t%d\t%d\t%d\n", s[3*n], s[3*n+2], s[3*(n+zp[n])], s[3*(n+xp[n]+2)], plaqueta_xy(n,s));
    return 0;
}