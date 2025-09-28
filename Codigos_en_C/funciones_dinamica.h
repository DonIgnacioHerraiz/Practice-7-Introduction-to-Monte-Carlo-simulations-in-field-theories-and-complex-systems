#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "random.h"
#include "funciones_red.h"

extern int xp[];
extern int yp[];
extern int zp[];
extern int xm[];
extern int ym[];
extern int zm[];

void crea_configuracionInicial( int flag, int *s);
void lee_configuracionInicial(int *s, char *input_file);
int arista_aleatoria();
void posicion_plaquetas(int arista_aleatoria, int *posiciones);
int indice_cociente_prob(int plaquetas[], int posiciones[]);
void vector_cociente_prob(double *vector);
int un_paso_metropolis(int *aristas, int *plaquetas, int arista, double probabilidades[]);
void N_pasos_metropolis(int N, int *aristas, int *plaquetas, double probabilidades[], int *aceptadas);