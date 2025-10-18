#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "parametros.h"

#define NVAR 11  // número de variables

void calcular_medias_y_error_append(const char *input_path, int N, const char *output_path) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        printf("❌ No se pudo abrir el archivo de entrada: %s\n", input_path);
        return;
    }

    double *sum = calloc(NVAR, sizeof(double));
    double *sum2 = calloc(NVAR, sizeof(double));
    if (!sum || !sum2) {
        printf("❌ Error al reservar memoria.\n");
        fclose(fin);
        free(sum);
        free(sum2);
        return;
    }

    int n_read = 0;
    double tiempo;
    double vars[NVAR];

    while (n_read < N && !feof(fin)) {
        int ncol = fscanf(fin, "%lf", &tiempo);
        for (int i = 0; i < NVAR; i++)
            ncol += fscanf(fin, "%lf", &vars[i]);

        if (ncol != NVAR + 1) break; // línea incompleta

        for (int i = 0; i < NVAR; i++) {
            sum[i]  += vars[i];
            sum2[i] += vars[i] * vars[i];
        }
        n_read++;
    }

    fclose(fin);

    if (n_read == 0) {
        printf("⚠️ No se leyeron filas válidas de %s\n", input_path);
        free(sum);
        free(sum2);
        return;
    }

    FILE *fout = fopen(output_path, "a");
    if (!fout) {
        printf("❌ No se pudo abrir o crear el archivo de salida: %s\n", output_path);
        free(sum);
        free(sum2);
        return;
    }

    // Escribir N como primera columna
    fprintf(fout, "%d", N);

    for (int i = 0; i < NVAR; i++) {
        double mean = sum[i] / n_read;
        double var  = (sum2[i] / n_read) - mean * mean;
        double std_error = (var > 0.0) ? sqrt(var / n_read) : 0.0; // desviación de la media
        fprintf(fout, "\t%.8f\t%.8f", mean, std_error);
    }
    fprintf(fout, "\n");

    fclose(fout);
    free(sum);
    free(sum2);


}


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NVAR 11

void generar_log_medias(const char *input_path, const char *output_path) {
    FILE *fin = fopen(input_path, "r");
    if (!fin) {
        printf("❌ No se pudo abrir el archivo de entrada: %s\n", input_path);
        return;
    }

    FILE *fout = fopen(output_path, "w");
    if (!fout) {
        printf("❌ No se pudo crear el archivo de salida: %s\n", output_path);
        fclose(fin);
        return;
    }

    fprintf(fout, "# Archivo generado automáticamente a partir de %s\n", input_path);
    fprintf(fout, "# Contiene log(media) y su error propagado (err_log = err/media)\n\n");

    // Reservamos arrays temporales
    double N;
    double media[NVAR];
    double err[NVAR];

    // Leemos fila a fila
    while (!feof(fin)) {
        int leidos = fscanf(fin, "%lf", &N);
        if (leidos != 1) break;

        for (int i = 0; i < NVAR; i++) {
            if (fscanf(fin, "%lf %lf", &media[i], &err[i]) != 2) {
                fclose(fin);
                fclose(fout);
                printf("❌ Error leyendo columnas en %s\n", input_path);
                return;
            }
        }

        // Para cada variable, escribimos su log(media) y error propagado
        for (int i = 0; i < NVAR; i++) {
            fprintf(fout, "------ VARIABLE %d ------\n", i+1);

            if (media[i] <= 0) {
                fprintf(fout, "⚠️ Media no positiva (%.6e), no se puede tomar log\n\n", media[i]);
                continue;
            }

            double logm = log(media[i]);
            double err_log = fabs(err[i] / media[i]);

            fprintf(fout, "log(Media)\t%.10f\n", logm);
            fprintf(fout, "err(log(Media))\t%.10f\n\n", err_log);
        }

        fprintf(fout, "---------------------------------\n\n");
    }

    fclose(fin);
    fclose(fout);
    printf("✅ Archivo creado correctamente: %s\n", output_path);
}


int main() {
    for (int N = 10000; N <= 30000; N++) {
        if (beta == 0.72) {
            calcular_medias_y_error_append(
                "Resultados_simulacion/MAIN/0.72/EVOLUCION/NASIO/I_0.txt",
                N,
                "MEDIAS.txt"
            );
        } else if (beta == 0.8) {
            calcular_medias_y_error_append(
                "Resultados_simulacion/MAIN/0.80/EVOLUCION/NASIO/I_0.txt",
                N,
                "MEDIAS_0_80.txt"
            );
        }
    }
    /*
    if (beta == 0.80) {
        generar_log_medias("MEDIAS_0_80.txt", "LOGS_0_80.txt");
    } else {
        generar_log_medias("MEDIAS.txt", "LOGS.txt");
    }
    */
    return 0;
}
