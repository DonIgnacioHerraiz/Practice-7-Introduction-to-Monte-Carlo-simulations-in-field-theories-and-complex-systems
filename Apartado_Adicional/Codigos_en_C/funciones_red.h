#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "random.h"
#include "parametros.h"

// Arrays globales de vecinos
extern int xp[L*L*L];
extern int yp[L*L*L];
extern int zp[L*L*L];
extern int xm[L*L*L];
extern int ym[L*L*L];
extern int zm[L*L*L];

void inicializa_vectores_de_vecinos(void);

int plaqueta_xy(int Nodo, int *aristas);

int plaqueta_xz(int Nodo, int *aristas);

int plaqueta_yz(int Nodo, int *aristas);

void dame_plaquetas(int *aristas, int *plaquetas);

void coordenadas_nodo(int Nodo, int *x, int *y, int *z);

double energia_normalizada(int *plaquetas);

void desviacion_estandar(int n, double datos[], double *media, double *desviacion);

int magnetizacion(int *aristas);

int vecino_n_xp(int Nodo, int n);

int vecino_n_yp(int Nodo, int n);

int vecino_n_zp(int Nodo, int n);

double una_fila_x_loop_z(int Nodo_inicial, int *aristas, int n);  

double una_fila_y_loop_x(int Nodo_inicial, int *aristas, int n);

double una_fila_z_loop_y(int Nodo_inicial, int *aristas, int n); 

int Wilson_loop_x(int Nodo_inicial, int *aristas, int n);    

int Wilson_loop_y(int Nodo_inicial, int *aristas, int n);

void dame_wilsons_nn(int *aristas, int *wilsons, int n);

int Wilson_loop_z(int Nodo_inicial, int *aristas, int n);  


int un_loop_z(int Nodo_inicial, int *arista, int n);

int un_loop_y(int Nodo_inicial, int *arista, int n);

int un_loop_x(int Nodo_inicial, int *arista, int n);

double prom_Wilson_loops(int n, int *aristas);


void inicializa_nodos_wilson( int n, int m, int nodos_wilson[][m][2]);

void precalcula_tabla_spin(double beta_val, double *tabla_spin);

void precalcula_tabla_bloque(double beta_val, double tabla_bloque[4][4][2][2][2]);

void promedio_bloque(double beta_val, double tabla_promedio[4][4][2]);

int indice_lado(int *plaquetas, int *aristas, int arista_actual);

void indices_esquinas(int *aristas, int *plaquetas, int nodo, int plano, int arista_1, int arista_2, int N_esquina, int *inidce_S1, int *indicce_S2, int *indice_Up);

void dame_O_n(int *aristas, int *plaquetas, double *O, int n, double tabla_promedio[4][4][2], double tabla_spin[5]);

double un_loop_O_x(int Nodo_inicial, int *aristas, int *plaquetas, int n, double tabla_promedio[4][4][2], double tabla_spin[5]);

double un_loop_O_y(int Nodo_inicial, int *aristas, int *plaquetas, int n, double tabla_promedio[4][4][2], double tabla_spin[5]);

double un_loop_O_z(int Nodo_inicial, int *aristas, int *plaquetas, int n, double tabla_promedio[4][4][2], double tabla_spin[5]);