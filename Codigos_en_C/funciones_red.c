#include "funciones_red.h"

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
        Nodo=Nodo+yp[Nodo];
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


