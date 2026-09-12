#include "funciones_red.h"
#include "funciones_dinamica.h"

// Definición de los arrays globales
int xp[L*L*L];
int yp[L*L*L];
int zp[L*L*L];
int xm[L*L*L];
int ym[L*L*L];
int zm[L*L*L];


void inicializa_vectores_de_vecinos(void) {

    for(int i = 0; i < L*L*L; i++) {
        xp[i] = 1;
        yp[i] = L;
        xm[i] = -1;
        ym[i] = -L;
        zp[i] = L*L;
        zm[i] = -L*L;
    }


    for(int i=0;i<L*L;i++){
        xp[L-1+L*i]=-(L-1);
        xm[L*i]=L-1;
    }

    for(int j=0;j<L;j++){        
        for(int k=0;k<L;k++){
            yp[L*(L-1)+L*L*j+k]=-L*(L-1);
            ym[L*L*j+k]=L*(L-1);
        }
    }

    for(int m=0;m<L;m++){
        for(int n=0;n<L;n++){
                zp[L*m+n+L*L*(L-1)]=-L*L*(L-1);
                zm[L*m+n]=L*L*(L-1);
        }
    }
    
}

int plaqueta_xy(int Nodo, int *aristas){
    int x, y, z;
    int s_nx = aristas[3*Nodo];
    int s_ny = aristas[3*Nodo+1];
    int s_n_mas_x_y = aristas[3*(Nodo + xp[Nodo]) + 1];
    int s_n_mas_y_x = aristas[3*(Nodo + yp[Nodo])];
    return s_nx * s_n_mas_x_y * s_ny * s_n_mas_y_x;
}

int plaqueta_xz(int Nodo, int *aristas){
    int x, y, z;
    int s_nx = aristas[3*Nodo];
    int s_nz = aristas[3*Nodo+2];
    int s_n_mas_x_z = aristas[3*(Nodo + xp[Nodo]) + 2];
    int s_n_mas_z_x = aristas[3*(Nodo + zp[Nodo])];
    return s_nx * s_n_mas_x_z * s_nz * s_n_mas_z_x;
}

int plaqueta_yz(int Nodo, int *aristas){
    int x, y, z;
    int s_ny = aristas[3*Nodo+1];
    int s_nz = aristas[3*Nodo+2];
    int s_n_mas_y_z = aristas[3*(Nodo + yp[Nodo]) + 2];
    int s_n_mas_z_y = aristas[3*(Nodo + zp[Nodo]) + 1];
    return s_ny * s_n_mas_y_z * s_nz * s_n_mas_z_y;
}

void dame_plaquetas(int *aristas, int *plaquetas){
    int V=L*L*L;
    for(int i=0;i<V;i++){
        plaquetas[3*i]=plaqueta_xy(i,aristas);
        plaquetas[3*i+1]=plaqueta_xz(i,aristas);
        plaquetas[3*i+2]=plaqueta_yz(i,aristas);
    }
}

void coordenadas_nodo(int Nodo, int *x, int *y, int *z){
    *z = Nodo/(L*L);
    *y = (Nodo - (*z)*L*L)/L;
    *x = Nodo - (*z)*L*L - (*y)*L;
}

double energia_normalizada(int *plaquetas){
    int V=L*L*L;
    double suma=0.0;
    for(int i=0;i<V;i++){
        suma=suma+plaquetas[3*i]+plaquetas[3*i+1]+plaquetas[3*i+2];
    }
    return (-J*suma);
}

void desviacion_estandar(int n, double datos[], double *media, double *desviacion){
    double suma=0.0;

    for(int i=0;i<n;i++){
        suma+=datos[i];
    }
    *media=suma/n;
    suma=0.0;

    
    for(int i=0;i<n;i++){
        suma+=(*media-datos[i])*(*media-datos[i]);
    }
    
    *desviacion=sqrt(suma/n);
}   

int magnetizacion(int *aristas){
    int V=3*L*L*L;
    int suma=0;
    for(int i=0;i<V;i++){
        suma=suma+aristas[i];
    }
    return suma;
}

int vecino_n_xp(int Nodo, int n){
    for(int i=0;i<n;i++){
        Nodo=Nodo+xp[Nodo];
    }
    return Nodo;
}

int vecino_n_yp(int Nodo, int n){
    for(int i=0;i<n;i++){
        Nodo=Nodo+yp[Nodo];
    }
    return Nodo;
}

int vecino_n_zp(int Nodo, int n){
    for(int i=0;i<n;i++){
        Nodo=Nodo+zp[Nodo];
    }
    return Nodo;
}


int un_loop_z(int Nodo_inicial, int *arista, int n){
    int lado_sur=1;
    int lado_oeste=1;
    int Nodo_sur=Nodo_inicial;
    int Nodo_oeste=Nodo_inicial;
    int x_sur, y_sur, z_sur;
    int x_oes, y_oes, z_oes;
    for(int i=0;i<n;i++){
        lado_sur=lado_sur*arista[3*(Nodo_sur)];
        lado_oeste=lado_oeste*arista[3*(Nodo_oeste)+1];
        Nodo_sur=Nodo_sur+xp[Nodo_sur];
        Nodo_oeste=Nodo_oeste+yp[Nodo_oeste];
    }
    int lado_norte=1;
    int lado_este=1;
    int Nodo_norte=Nodo_oeste;
    int Nodo_este=Nodo_sur;

    for(int i=0;i<n;i++){
        lado_norte=lado_norte*arista[3*(Nodo_norte)];
        lado_este=lado_este*arista[3*(Nodo_este)+1];
        Nodo_norte=Nodo_norte+xp[Nodo_norte];
        Nodo_este=Nodo_este+yp[Nodo_este];
    }
    return lado_sur*lado_oeste*lado_norte*lado_este;
}

int un_loop_y(int Nodo_inicial, int *arista, int n){
    int lado_sur=1;
    int lado_oeste=1;
    int Nodo_sur=Nodo_inicial;
    int Nodo_oeste=Nodo_inicial;
    for(int i=0;i<n;i++){
        lado_sur=lado_sur*arista[3*(Nodo_sur)+2];
        lado_oeste=lado_oeste*arista[3*(Nodo_oeste)];
        Nodo_sur=Nodo_sur+zp[Nodo_sur];
        Nodo_oeste=Nodo_oeste+xp[Nodo_oeste];
    }
    int lado_norte=1;
    int lado_este=1;
    int Nodo_norte=Nodo_oeste;
    int Nodo_este=Nodo_sur;

    for(int i=0;i<n;i++){
        lado_norte=lado_norte*arista[3*(Nodo_norte)+2];
        lado_este=lado_este*arista[3*(Nodo_este)];
        Nodo_norte=Nodo_norte+zp[Nodo_norte];
        Nodo_este=Nodo_este+xp[Nodo_este];
    }
    return lado_sur*lado_oeste*lado_norte*lado_este;
}

int un_loop_x(int Nodo_inicial, int *arista, int n){
    int lado_sur=1;
    int lado_oeste=1;
    int Nodo_sur=Nodo_inicial;
    int Nodo_oeste=Nodo_inicial;
    for(int i=0;i<n;i++){
        lado_sur=lado_sur*arista[3*(Nodo_sur)+1];
        lado_oeste=lado_oeste*arista[3*(Nodo_oeste)+2];
        Nodo_sur=Nodo_sur+yp[Nodo_sur];
        Nodo_oeste=Nodo_oeste+zp[Nodo_oeste];
    }
    int lado_norte=1;
    int lado_este=1;
    int Nodo_norte=Nodo_oeste;
    int Nodo_este=Nodo_sur;

    for(int i=0;i<n;i++){
        lado_norte=lado_norte*arista[3*(Nodo_norte)+1];
        lado_este=lado_este*arista[3*(Nodo_este)+2];
        Nodo_norte=Nodo_norte+yp[Nodo_norte];
        Nodo_este=Nodo_este+zp[Nodo_este];
    }
    return lado_sur*lado_oeste*lado_norte*lado_este;
}

double prom_Wilson_loops(int n, int *aristas){

    int promedio=0;
    int Nodo_actual_z=0;
    int Nodo_actual_y=0;
    int Nodo_actual_x=0;

    for(int i=0;i<L-1;i++){
        promedio=promedio+un_loop_z(Nodo_actual_z,aristas,n)+un_loop_z(Nodo_actual_y,aristas,n)+un_loop_z(Nodo_actual_x,aristas,n);
        Nodo_actual_z=Nodo_actual_z+zp[Nodo_actual_z];
        Nodo_actual_x=Nodo_actual_x+xp[Nodo_actual_x];
        Nodo_actual_y=Nodo_actual_y+yp[Nodo_actual_y];
    }
    return promedio/(3*L);
}


void inicializa_nodos_wilson( int n, int m, int nodos_wilson[][m][2]){
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            nodos_wilson[i][j][0]=generador_plano(0,L*L*L);
            nodos_wilson[i][j][1]=generador_plano(0,3);
        }
    }
}

void dame_wilsons_nn(int *aristas, int *wilsons, int n){
    int V=L*L*L;
    for(int i=0;i<V;i++){
        wilsons[3*i]=un_loop_x(i,aristas,n);
        wilsons[3*i+1]=un_loop_y(i,aristas,n);
        wilsons[3*i+2]=un_loop_z(i,aristas,n);
    }
}

//Funcion para precalcular la tabla de valores de spin para la nueva variable
void precalcula_tabla_spin(double beta_val, double *tabla_spin){
    double S_l;
    for (int i=0;i<5;i++){
        S_l = (double) 2.0*i-4.0;
        tabla_spin[i]=tanh(beta_val*S_l*J);
    }
}

//Precálculo del promedio local para bloques de esquina
void precalcula_tabla_bloque(double beta_val, double tabla_bloque[4][4][2][2][2]){
    int i, j, k, l, m;
    double S1, S2, Up, sigma1, sigma2;

    // Recorremos todas las configuraciones posibles del entorno (S1, S2, Up)
    for (i = 0; i < 4; i++) {
        S1 = (double)(i * 2 - 3); 
        for (j = 0; j < 4; j++) {
            S2 = (double)(j * 2 - 3); 
            for (k = 0; k < 2; k++) {
                Up = (double)(k * 2 - 1);
                for (l = 0; l < 2; l++) {     // sigma1 actual
                    sigma1=(double)(l*2-1);
                    for (m = 0; m < 2; m++) { // sigma2 actual
                        sigma2=(double)(m*2-1);
                        tabla_bloque[i][j][k][l][m]=exp(beta_val*J*(S1*sigma1 + S2*sigma2 + Up*sigma1*sigma2));
                    }
                }
            }
        }
    }
}

//Cálculo del promedio local para bloques de esquina
void promedio_bloque(double beta_val, double tabla_promedio[4][4][2]){
    int i, j, k, l, m;
    double numerador, denominador;
    double S1, S2, Up;
    double tabla_bloque[4][4][2][2][2];
    double W1, W2, W3, W4;// W1=W++, W2=W-+, W3=W+-, W4=W--
    precalcula_tabla_bloque(beta_val, tabla_bloque);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 2; k++) {
                W1 = tabla_bloque[i][j][k][1][1]; //W++
                W2 = tabla_bloque[i][j][k][0][1]; //W-+
                W3 = tabla_bloque[i][j][k][1][0]; //W+-
                W4 = tabla_bloque[i][j][k][0][0]; //W--
                numerador= W1 - W2 - W3 + W4;
                denominador= W1 + W2 + W3 + W4;
                tabla_promedio[i][j][k] = numerador / denominador;
            }
        }
    }
}

int indice_lado(int *plaquetas, int *aristas, int arista_actual){
    int Sl=0;
    int indices_plaquetas[4];

    //ESTA FUNCION TE DA EL INDICE DE LAS 4 PLAQUETAS A LAS QUE PERTENECE ARISTA_ACTUAL
    posicion_plaquetas(arista_actual,indices_plaquetas);

    for(int i=0;i<4;i++){//SUMAMOS EL VALOR DE LAS 4 PLAQUETAS
        Sl+=plaquetas[indices_plaquetas[i]];
    }
    // SL debe normalizarse por el valor de la arista (±1), no por su índice
    Sl /= aristas[arista_actual];
    //DEVOLVEMOS EL INDICE PARA LA FUNCION PRECALCULA_TABLA_SPIN
    return (Sl+4)/2;
}

void indices_esquinas(int *aristas, int *plaquetas, int nodo, int plano, int arista_1, int arista_2, int N_esquina, int *inidce_S1, int *indicce_S2, int *indice_Up){
    int S1=0,S2=0,Up=0;
    int posiciones_plaquetas_1[4], posiciones_plaquetas_2[4], indice_plaqueta;

    posicion_plaquetas(arista_1,posiciones_plaquetas_1);
    posicion_plaquetas(arista_2,posiciones_plaquetas_2);

    for(int i=0;i<4;i++){
        S1+=plaquetas[posiciones_plaquetas_1[i]];
        S2+=plaquetas[posiciones_plaquetas_2[i]];
    }

    switch (plano)
    {
    case 0:
        switch (N_esquina)
        {
        case 0:
            indice_plaqueta=3*nodo;
            break;
        case 1:
            indice_plaqueta=3*(nodo+xm[nodo]);
            break;
        case 2:
            nodo=nodo+ym[nodo];
            indice_plaqueta=3*(nodo+xm[nodo]);
            break;
        case 3:
            indice_plaqueta=3*(nodo+ym[nodo]);
            break;
        default:
            break;
        }break;
    case 1:
        switch (N_esquina)
        {
        case 0:
            indice_plaqueta=3*nodo+1;
            break;
        case 1:
            indice_plaqueta=3*(nodo+xm[nodo])+1;
            break;
        case 2:
            nodo=nodo+zm[nodo];
            indice_plaqueta=3*(nodo+xm[nodo])+1;
            break;
        case 3:
            indice_plaqueta=3*(nodo+zm[nodo])+1;
            break;
        default:
            break;
        }break;
    case 2:
        switch (N_esquina)
        {
        case 0:
            indice_plaqueta=3*nodo+2;
            break;
        case 1:
            indice_plaqueta=3*(nodo+ym[nodo])+2;
            break;
        case 2:
            nodo=nodo+zm[nodo];
            indice_plaqueta=3*(nodo+ym[nodo])+2;
            break;
        case 3:
            indice_plaqueta=3*(nodo+zm[nodo])+2;
            break;
        default:
            break;
        }break;
        
    default:
        break;
    }

    S1-=plaquetas[indice_plaqueta];
    S2-=plaquetas[indice_plaqueta];

    S1/=aristas[arista_1];
    S2/=aristas[arista_2];
    Up=plaquetas[indice_plaqueta]/aristas[arista_1]/aristas[arista_2];


    *inidce_S1 = (S1+3)/2;
    *indicce_S2 = (S2+3)/2;
    *indice_Up = (Up+1)/2;
}

void dame_O_n(int *aristas, int *plaquetas, double *O, int n, double tabla_promedio[4][4][2], double tabla_spin[5]){
    int V=L*L*L;
    for(int i=0;i<V;i++){
        O[3*i]=un_loop_O_x(i,aristas,plaquetas,n,tabla_promedio,tabla_spin);
        O[3*i+1]=un_loop_O_y(i,aristas,plaquetas,n,tabla_promedio,tabla_spin);
        O[3*i+2]=un_loop_O_z(i,aristas,plaquetas,n,tabla_promedio,tabla_spin);
    }
}

double un_loop_O_x(int Nodo_inicial, int *aristas, int *plaquetas, int n, double tabla_promedio[4][4][2], double tabla_spin[5]){
    double On=1.0;//lo defino como 1 porque voy a multiplicar las esperanzas locales
    int indice_S1, indice_S2, indice_Up;

    int nodo=Nodo_inicial;

    indices_esquinas(aristas,plaquetas,nodo,0,3*nodo,3*nodo+1,0,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo=nodo+xp[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo)];
        nodo+=xp[nodo];
    }

    nodo+=xp[nodo];
    indices_esquinas(aristas,plaquetas,nodo,0,3*(nodo+xm[nodo]),3*nodo+1,1,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=yp[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+1)];
        nodo+=yp[nodo];
    }

    nodo+=yp[nodo];
    indices_esquinas(aristas,plaquetas,nodo,0,3*(nodo+xm[nodo]),3*(nodo+ym[nodo])+1,2,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=xm[nodo];
    nodo+=xm[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo)];
        nodo+=xm[nodo];
    }

    indices_esquinas(aristas,plaquetas,nodo,0,3*nodo,3*(nodo+ym[nodo])+1,3,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=ym[nodo];
    nodo+=ym[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+1)];
        nodo+=ym[nodo];
    }

    return On;
}

double un_loop_O_y(int Nodo_inicial, int *aristas, int *plaquetas, int n, double tabla_promedio[4][4][2], double tabla_spin[5]){
    double On=1.0;//lo defino como 1 porque voy a multiplicar las esperanzas locales
    int indice_S1, indice_S2, indice_Up;

    int nodo=Nodo_inicial;

    indices_esquinas(aristas,plaquetas,nodo,1,3*nodo,3*nodo+2,0,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo=nodo+xp[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo)];
        nodo+=xp[nodo];
    }

    nodo+=xp[nodo];
    indices_esquinas(aristas,plaquetas,nodo,1,3*(nodo+xm[nodo]),3*nodo+2,1,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=zp[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+2)];
        nodo+=zp[nodo];
    }

    nodo+=zp[nodo];
    indices_esquinas(aristas,plaquetas,nodo,1,3*(nodo+xm[nodo]),3*(nodo+zm[nodo])+2,2,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=xm[nodo];
    nodo+=xm[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo)];
        nodo+=xm[nodo];
    }

    indices_esquinas(aristas,plaquetas,nodo,1,3*nodo,3*(nodo+zm[nodo])+2,3,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=zm[nodo];
    nodo+=zm[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+2)];
        nodo+=zm[nodo];
    }

    return On;
}

double un_loop_O_z(int Nodo_inicial, int *aristas, int *plaquetas, int n, double tabla_promedio[4][4][2], double tabla_spin[5]){
    double On=1.0;//lo defino como 1 porque voy a multiplicar las esperanzas locales
    int indice_S1, indice_S2, indice_Up;

    int nodo=Nodo_inicial;

    indices_esquinas(aristas,plaquetas,nodo,2,3*nodo+1,3*nodo+2,0,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo=nodo+yp[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+1)];
        nodo+=yp[nodo];
    }

    nodo+=yp[nodo];
    indices_esquinas(aristas,plaquetas,nodo,2,3*(nodo+ym[nodo])+1,3*nodo+2,1,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=zp[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+2)];
        nodo+=zp[nodo];
    }

    nodo+=zp[nodo];
    indices_esquinas(aristas,plaquetas,nodo,2,3*(nodo+ym[nodo])+1,3*(nodo+zm[nodo])+2,2,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=ym[nodo];
    nodo+=ym[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+1)];
        nodo+=ym[nodo];
    }

    indices_esquinas(aristas,plaquetas,nodo,2,3*nodo+1,3*(nodo+zm[nodo])+2,3,&indice_S1,&indice_S2,&indice_Up);
    On=On*tabla_promedio[indice_S1][indice_S2][indice_Up];
    nodo+=zm[nodo];
    nodo+=zm[nodo];

    for(int i=0;i<(n-2);i++){
        On=On*tabla_spin[indice_lado(plaquetas, aristas, 3*nodo+2)];
        nodo+=zm[nodo];
    }

    return On;
}