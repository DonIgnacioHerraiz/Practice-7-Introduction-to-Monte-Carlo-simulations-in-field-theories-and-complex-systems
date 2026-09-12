#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "funciones_red.h"
#include "funciones_dinamica.h"
#include "random.h"
#include "parametros.h"

// Colores para que la salida por terminal sea "guay" y fácil de leer
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define CYN   "\x1B[36m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"

int main() {
    // --- 1. PREPARACIÓN ---
    inicializa_PR(12345);
    inicializa_vectores_de_vecinos();

    // Parámetros de la prueba
    double beta_test = 0.72;
    int n_loop = 5; // Loop de 3x3
    int V = 3 * L * L * L; // Volumen total de enlaces

    // Memoria
    int *s = malloc(V * sizeof(int));
    int *plaquetas = malloc(V * sizeof(int));
    double *O_n = malloc(V * sizeof(double)); // ¡IMPORTANTE: DOUBLE!
    
    // Tablas de Parisi
    double tabla_spin[5];
    double tabla_bloque[4][4][2];

    // Precalcular Tablas
    precalcula_tabla_spin(beta_test, tabla_spin);
    printf("\n\n************TABLA SPIN**************\n\n");
    for(int i=0;i<5;i++)printf("\n%f",tabla_spin[i]);
    promedio_bloque(beta_test, tabla_bloque);
    printf("\n\n************TABLA BLOQUE**************\n\n");
    for(int i=0;i<4;i++){printf("\n\n\n");
    for(int j=0;j<4;j++){printf("\n");
    for(int k=0;k<2;k++)
    printf("\t%f",tabla_bloque[i][j][k]);
    }}

    printf("\n" YEL "==========================================================\n");
    printf("      TEST FINAL DE VALIDACIÓN: VARIABLE DE PARISI\n");
    printf("==========================================================" RESET "\n");
    printf("Params: Beta=%.2f | Loop=%dx%d | L=%d\n\n", beta_test, n_loop, n_loop, L);

    // ========================================================================
    // ESCENARIO 1: EL VACÍO PERFECTO (COLD START)
    // ========================================================================
    printf(CYN ">>> ESCENARIO 1: RED CONGELADA (Todos los espines = +1)\n" RESET);
    
    // 1. Llenar red con +1
    for(int i=0; i<V; i++) s[i] = 1;
    dame_plaquetas(s, plaquetas); // Actualizar plaquetas (todas valdrán +1)

    // 2. Ejecutar tu función
    dame_O_n(s, plaquetas, O_n, n_loop, tabla_bloque, tabla_spin);

    // 3. Calcular VALOR TEÓRICO a mano
    // En una red de +1, todas las grapas (staples) suman 4.
    // Indice para S=4 es (4+4)/2 = 4.
    double val_lado_teorico = tabla_spin[4]; // tanh(beta * J * 4)

    // En una esquina de +1, S1=3, S2=3, Up=1.
    // Indices: i=(3+3)/2=3, j=(3+3)/2=3, k=(1+1)/2=1
    double val_esquina_teorico = tabla_bloque[3][3][1];

    // Un loop NxN tiene: 4 esquinas y 4*(N-2) lados rectos.
    int num_esquinas = 4;
    int num_lados = 4 * (n_loop - 2);

    double O_teorico = pow(val_lado_teorico, num_lados) * pow(val_esquina_teorico, num_esquinas);

    // 4. Comparar
    printf("   -> Valor Lado (tanh):   %.6f\n", val_lado_teorico);
    printf("   -> Valor Esquina (avg): %.6f\n", val_esquina_teorico);
    printf(YEL "   -> RESULTADO TEÓRICO ESPERADO: %.8f\n" RESET, O_teorico);
    printf(CYN "   -> RESULTADO DE 'dame_O_n':    %.8f\n" RESET, O_n[0]); // Miramos el primer loop

    double error = fabs(O_n[0] - O_teorico);
    if(error < 1e-9) {
        printf(GRN "[PASS] ¡PERFECTO! La simulación coincide con la teoría exacta.\n" RESET);
    } else {
        printf(RED "[FAIL] Discrepancia de %e. Algo falla en la geometría.\n" RESET, error);
    }

    // Check de Simetría (X, Y, Z deben dar lo mismo en el vacío)
    if(fabs(O_n[0] - O_n[1]) < 1e-9 && fabs(O_n[1] - O_n[2]) < 1e-9) {
        printf(GRN "[PASS] Simetría Espacial OK (X=Y=Z).\n" RESET);
    } else {
        printf(RED "[FAIL] Error de Simetría: X=%.4f, Y=%.4f, Z=%.4f\n" RESET, O_n[0], O_n[1], O_n[2]);
    }

    printf("\n");

    // ========================================================================
    // ESCENARIO 2: EL CAOS (HOT START)
    // ========================================================================
    printf(CYN ">>> ESCENARIO 2: RED ALEATORIA (Hot Start)\n" RESET);
    printf("Generando configuración aleatoria...\n");
    
    crea_configuracion(0, s); // 0 = aleatoria
    dame_plaquetas(s, plaquetas); // IMPORTANTE: Actualizar plaquetas tras cambiar espines
    
    // Ejecutar tu función en entorno real
    dame_O_n(s, plaquetas, O_n, n_loop, tabla_bloque, tabla_spin);

    printf("Muestra de resultados (Primeros 5 nodos):\n");
    printf("Nodo |   Loop X   |   Loop Y   |   Loop Z   |\n");
    printf("-----|------------|------------|------------|\n");
    
    int ceros_detectados = 0;
    for(int i=0; i<5; i++) {
        printf("  %d  |  % .8f  |  % .8f  |  % .8f  |\n", 
               i, O_n[3*i], O_n[3*i+1], O_n[3*i+2]);
        
        if(O_n[3*i] == 0.0) ceros_detectados++;
    }

    if(ceros_detectados == 0) {
        printf(GRN "\n[PASS] Datos Correctos: Se observan valores decimales (doubles).\n" RESET);
        printf("       (Esto confirma que no hay truncamiento a int).\n");
    } else {
        printf(RED "\n[FAIL] ¡ALERTA! Se detectaron ceros exactos. Posible error de tipo int.\n" RESET);
    }

    printf("\n" YEL "==========================================================\n");
    printf("      FIN DEL TEST\n");
    printf("==========================================================" RESET "\n");

    // Limpieza
    free(s);
    free(plaquetas);
    free(O_n);

    return 0;
}

/*
#include "funciones_dinamica.h"
#include "funciones_red.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    inicializa_PR(12345);
    inicializa_vectores_de_vecinos();

    int V = L*L*L;
    int *s = malloc(sizeof(int)*3*V);
    int *plaquetas = malloc(sizeof(int)*3*V);
    if(!s || !plaquetas){
        printf("Error allocating arrays\n");
        return 1;
    }

    // Crear configuración inicial (cold = 0)
    crea_configuracionInicial(0, s);
    dame_plaquetas(s, plaquetas);

    // Precalcular tablas necesarias
    double tabla_spin[5];
    precalcula_tabla_spin(beta, tabla_spin);

    double tabla_promedio[4][4][2];
    promedio_bloque(beta, tabla_promedio);

    int n = 1;
    // Llamadas a las nuevas funciones
    double ox = un_loop_O_x(0, s, plaquetas, n, tabla_promedio, tabla_spin);
    double oy = un_loop_O_y(0, s, plaquetas, n, tabla_promedio, tabla_spin);
    double oz = un_loop_O_z(0, s, plaquetas, n, tabla_promedio, tabla_spin);

    printf("un_loop_O_x(0,1) = %f\n", ox);
    printf("un_loop_O_y(0,1) = %f\n", oy);
    printf("un_loop_O_z(0,1) = %f\n", oz);

    double *O = malloc(sizeof(int)*V);
    if(O){
        dame_O_n(s, plaquetas, O, n, tabla_promedio, tabla_spin);
        printf("First 20 values of O (or fewer):\n");
        for(int i=0;i<20 && i<V;i++) printf("%d ", O[i]);
        printf("\n");
        free(O);
    } else {
        printf("No se pudo reservar memoria para O\n");
    }

    free(s);
    free(plaquetas);
    return 0;
}
*/
