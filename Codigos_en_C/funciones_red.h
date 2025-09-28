#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "random.h"
#include "parametros.h"

// Arrays globales de vecinos
extern int xp[L];
extern int yp[L];
extern int zp[L];
extern int xm[L];
extern int ym[L];
extern int zm[L];

void inicializa_vectores_de_vecinos(void);

int plaqueta_xy(int Nodo, int *aristas);

int plaqueta_xz(int Nodo, int *aristas);

int plaqueta_yz(int Nodo, int *aristas);

void coordenadas_nodo(int Nodo, int *x, int *y, int *z);

void dame_plaquetas(int *aristas, int *plaquetas);

