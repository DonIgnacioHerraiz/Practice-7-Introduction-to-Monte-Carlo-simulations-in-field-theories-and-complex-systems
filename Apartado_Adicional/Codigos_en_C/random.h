#pragma once

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#define PI 3.14159265358979323846

// Devuelve un número aleatorio uniforme en (0,1)
double fran(void);

// Inicializa el generador Parisi-Rapuano con una semilla
void inicializa_PR(int SEMILLA);

// Genera un número aleatorio en el rango [n1, n2)
double generador_plano(double n1,double n2);
