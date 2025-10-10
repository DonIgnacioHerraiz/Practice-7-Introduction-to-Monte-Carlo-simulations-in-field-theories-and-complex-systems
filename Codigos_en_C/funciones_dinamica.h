#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "random.h"
#include "funciones_red.h"

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

void crea_configuracion( int flag, int *s);
void estadistica(int *variable, double *media, double *desvest);

void crea_configuracionInicial( int flag, int *s);
void lee_configuracionInicial(int *s, char *input_file);
int arista_aleatoria();
int nodo_aleatorio();
int cambio_aristas_nodo(int *s);
void posicion_plaquetas(int arista_aleatoria, int *posiciones);
int indice_cociente_prob(int plaquetas[], int posiciones[]);
void vector_cociente_prob(double *vector);
int un_paso_metropolis(int *aristas, int *plaquetas, int arista, double probabilidades[]);
void N_pasos_metropolis(int N_sweeps, int *aristas, int *plaquetas, double probabilidades[], int *aceptadas);

void calculo_promedios_wilson(int *s, int *plaquetas, int n, int m, int nodos_wilson[n][m][2], int n_pasos, int n_pasos_entre_mediciones, int n_termalizacion, double probabilidades[5]);
void guarda_parametros(int n, int m, int n_pasos, int n_pasos_entre_mediciones, int n_termalizacion);

void dinamica_metropolis(int N_sweeps_entre_med, int N_medidas, double probabilidades[5], int *aristas, int *plaquetas
#ifdef correlacion
    , const char* filename_evolucion, const char* filename_parametros
#endif
    );
void crea_configuracionInicial_termalizacion( int flag, int *s);

double promedio(int *variable, int N);
void calcular_media_std(double *data, int N, double *media, double *std);
void crear_ventanas(int k_ini, int k_final, int N_ventana, int N_salto);
void crear_media_global(int k_ini, int k_final);

void dinamica_metropolis_main(
    int N_sweeps_entre_med,
    int N_medidas,
    double probabilidades[5],
    int *aristas,
    int *plaquetas, char *filename_aristas
);