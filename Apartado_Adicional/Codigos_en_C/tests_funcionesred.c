#include "funciones_red.h"

// ------------------------------------------------------------
// TEST 1: TABLA DE ESPINES (LADOS) -> Salida por Pantalla
// ------------------------------------------------------------
void test_tabla_spin_pantalla(double beta_val) {
    double tabla_spin[5];
    precalcula_tabla_spin(beta_val, tabla_spin);

    printf("\n===================================================\n");
    printf(" TEST 1: TABLA DE ESPINES (LADOS) - tanh(beta*J*S)\n");
    printf("===================================================\n");
    printf("Beta = %.2f, J = %.2f\n\n", beta_val, J);
    printf("Indice | S_l (Staples) | Valor (tanh)      | Check Simetria\n");
    printf("-------|---------------|-------------------|---------------\n");

    for (int i = 0; i < 5; i++) {
        double S_l = (double)(i * 2 - 4); // Mapeo inverso
        printf("   %d   |     %4.0f      | % .10f ", i, S_l, tabla_spin[i]);
        
        // Check visual de simetría
        if (i < 2) {
            double opuesto = tabla_spin[4-i];
            if (fabs(tabla_spin[i] + opuesto) < 1e-9) 
                printf("| OK (+% .4f)", opuesto);
            else 
                printf("| FAIL");
        } else if (i == 2) {
            if (fabs(tabla_spin[i]) < 1e-9) printf("| OK (Zero)");
        }
        printf("\n");
    }
    printf("\n");
}

// ------------------------------------------------------------
// TEST 2: PESOS CRUDOS (EXPONENCIALES) -> Fichero
// ------------------------------------------------------------
void test_fichero_pesos_raw(double beta_val) {
    double tabla_bloque[4][4][2][2][2];
    // Llamamos a tu función que calcula las exponenciales
    precalcula_tabla_bloque(beta_val, tabla_bloque);

    FILE *f = fopen("TESTS/test_pesos_boltzmann.txt", "w");
    if (!f) { perror("Error abriendo fichero raw"); return; }

    fprintf(f, "########################################################\n");
    fprintf(f, "# TEST 2: PESOS DE BOLTZMANN CRUDOS (W)\n");
    fprintf(f, "# Beta = %.2f, J = %.2f\n", beta_val, J);
    fprintf(f, "# W(s1, s2) = exp( beta * J * (S1*s1 + S2*s2 + Up*s1*s2) )\n");
    fprintf(f, "########################################################\n\n");

    // Iteramos sobre Up (Plaqueta compartida)
    for (int k = 0; k < 2; k++) {
        double Up = (double)(k * 2 - 1);
        fprintf(f, "====================================\n");
        fprintf(f, " CASO UP (Plaqueta Compartida) = %.0f\n", Up);
        fprintf(f, "====================================\n");
        fprintf(f, "%-5s %-5s | %-12s %-12s %-12s %-12s\n", "S1", "S2", "W(+,+)", "W(+,-)", "W(-,+)", "W(-,-)");
        fprintf(f, "------+-------+------------------------------------------------------\n");

        for (int i = 0; i < 4; i++) {
            double S1 = (double)(i * 2 - 3);
            for (int j = 0; j < 4; j++) {
                double S2 = (double)(j * 2 - 3);

                // Recuperamos los pesos usando los índices de espines
                // l=1 (sigma=1), l=0 (sigma=-1)
                double w_pp = tabla_bloque[i][j][k][1][1]; // ++
                double w_pm = tabla_bloque[i][j][k][1][0]; // +-
                double w_mp = tabla_bloque[i][j][k][0][1]; // -+
                double w_mm = tabla_bloque[i][j][k][0][0]; // --

                fprintf(f, " % 3.0f   % 3.0f   | %.5e  %.5e  %.5e  %.5e\n", 
                        S1, S2, w_pp, w_pm, w_mp, w_mm);
            }
            fprintf(f, "------+-------+------------------------------------------------------\n");
        }
        fprintf(f, "\n\n");
    }

    fclose(f);
    printf("-> Generado fichero: TESTS/test_pesos_boltzmann.txt\n");
}

// ------------------------------------------------------------
// TEST 3: PROMEDIOS FINALES (ESPERANZA LOCAL) -> Fichero
// ------------------------------------------------------------
void test_fichero_promedios(double beta_val) {
    double tabla_promedio[4][4][2]; // [S1][S2][Up]
    // Llamamos a tu función que calcula los promedios finales
    promedio_bloque(beta_val, tabla_promedio);

    FILE *f = fopen("TESTS/test_promedios_finales.txt", "w");
    if (!f) { perror("Error abriendo fichero promedios"); return; }

    fprintf(f, "########################################################\n");
    fprintf(f, "# TEST 3: PROMEDIOS TERMICOS LOCALES <sigma1*sigma2>\n");
    fprintf(f, "# Beta = %.2f, J = %.2f\n", beta_val, J);
    fprintf(f, "# Valores esperados entre -1.0 y 1.0\n");
    fprintf(f, "########################################################\n\n");

    for (int k = 0; k < 2; k++) {
        double Up = (double)(k * 2 - 1);
        fprintf(f, "VISTA MATRICIAL PARA UP = %.0f\n", Up);
        fprintf(f, "(Filas = S1, Columnas = S2)\n\n");
        
        // Cabecera de columnas (S2)
        fprintf(f, " S1\\S2 |");
        for(int j=0; j<4; j++) fprintf(f, "   % 2.0f      ", (double)(j*2-3));
        fprintf(f, "\n-------+");
        for(int j=0; j<4; j++) fprintf(f, "-------------");
        fprintf(f, "\n");

        // Filas (S1)
        for (int i = 0; i < 4; i++) {
            double S1 = (double)(i * 2 - 3);
            fprintf(f, "   % 2.0f  |", S1);
            
            for (int j = 0; j < 4; j++) {
                double val = tabla_promedio[i][j][k];
                fprintf(f, "  % .6f  ", val);
            }
            fprintf(f, "\n");
        }
        fprintf(f, "\nLeyenda:\n");
        fprintf(f, " * Si S1 y S2 son positivos, el valor debería ser cercano a 1.0 (Alineados)\n");
        fprintf(f, " * Si S1 y S2 son opuestos, el valor debería ser menor (o negativo)\n");
        fprintf(f, "\n\n");
    }

    fclose(f);
    printf("-> Generado fichero: TESTS/test_promedios_finales.txt\n");
}

int main() {
    double beta_test = 0.72; // Un valor típico de tus simulaciones
    
    // Ejecutar tests
    test_tabla_spin_pantalla(beta_test);
    test_fichero_pesos_raw(beta_test);
    test_fichero_promedios(beta_test);

    printf("\nTests finalizados. Revisa los ficheros generados.\n");
    return 0;
}