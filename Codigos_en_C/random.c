#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#define PI 3.14159265358979323846



// Variables que hay que definir para Parisi-Rapuano
#define NormRANu (2.3283063671E-10F)
unsigned int irr[256];
unsigned int ir1;
unsigned char ind_ran,ig1,ig2,ig3;


//Esta función devuelve un numero aleatorio uniforme en (0,1)
double fran(void) // Cambiado a double
{
    double r; // Cambiado a double
    ig1 = (ind_ran - 24) & 255; // Asegura que el índice esté en [0, 255]
    ig2 = (ind_ran - 55) & 255; // Asegura que el índice esté en [0, 255]
    ig3 = (ind_ran - 61) & 255; // Asegura que el índice esté en [0, 255]
    irr[ind_ran] = irr[ig1] + irr[ig2];
    ir1 = (irr[ind_ran] ^ irr[ig3]);
    ind_ran = (ind_ran + 1) & 255; // Incrementa y asegura que esté en [0, 255]
    r = ir1 * (double)NormRANu; // Asegura que el cálculo sea en double
    return r;
}
void inicializa_PR(int SEMILLA)
{
    int INI,FACTOR,SUM,i;

    srand(SEMILLA);

    INI=SEMILLA;
    FACTOR=67397;
    SUM=7364893;

    for(i=0;i<256;i++)
    {
        INI=(INI*FACTOR+SUM);
        irr[i]=INI;
    }
    ind_ran=ig1=ig2=ig3=0;
}


double generador_plano(double n1,double n2){
    return n1+(n2-n1)*fran();
}