#include "parametros.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAX_COLS 12
#define MAX_LINE 1024

void procesar_fichero(const char *ruta_fichero, FILE *out) {
    FILE *f = fopen(ruta_fichero, "r");
    if (!f) {
        perror("No se pudo abrir el fichero");
        return;
    }

    double suma[ MAX_COLS ] = {0.0};
    double suma_cuadrados[ MAX_COLS ] = {0.0};
    int n = 0;

    char linea[MAX_LINE];
    while (fgets(linea, sizeof(linea), f)) {
        double valores[MAX_COLS];
        int count = sscanf(linea,
            "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &valores[0], &valores[1], &valores[2], &valores[3],
            &valores[4], &valores[5], &valores[6], &valores[7],
            &valores[8], &valores[9], &valores[10], &valores[11]);

        if (count == MAX_COLS) {
            for (int j = 1; j <= 10; j++) {  // columnas 2 a 11 → índices 1 a 10
                suma[j] += valores[j];
                suma_cuadrados[j] += valores[j] * valores[j];
            }
            n++;
        }
    }

    if (n == 0) {
        fprintf(stderr, "El fichero %s está vacío o tiene formato inválido.\n", ruta_fichero);
        fclose(f);
        return;
    }

    // calcular estadísticas
    for (int j = 1; j <= 10; j++) {
        double media = suma[j] / n;
        double varianza = (suma_cuadrados[j] / n) - (media * media);
        double desv = sqrt(varianza > 0 ? varianza : 0);
        fprintf(out, "%d\t%.6f\t%.6f\n", j, media, desv);
    }

    fclose(f);
}

int main(void) {
    const char *folder_in;  
    const char *ruta_salida;
    
#ifdef Na
    if (beta == 0.72) {
        folder_in   = "Resultados_simulacion/MAIN/0.72/EVOLUCION/NASIO";
        ruta_salida   = "Resultados_simulacion/MAIN/0.72/PROMEDIOS_ERRORES/resultados_Na.txt";
    } else if (beta == 0.8) {
        folder_in   = "Resultados_simulacion/MAIN/0.8/EVOLUCION/NASIO";
        ruta_salida   = "Resultados_simulacion/MAIN/0.8/PROMEDIOS_ERRORES/resultados_Na.txt";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Ba)
    if (beta == 0.72) {
        folder_in   = "Resultados_simulacion/MAIN/0.72/EVOLUCION/BARRACHINA";
        ruta_salida   = "Resultados_simulacion/MAIN/0.72/PROMEDIOS_ERRORES/resultados_Ba.txt";
    } else if (beta == 0.8) {
        folder_in   = "Resultados_simulacion/MAIN/0.8/EVOLUCION/BARRACHINA";
        ruta_salida   = "Resultados_simulacion/MAIN/0.8/PROMEDIOS_ERRORES/resultados_Ba.txt";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Jo)
    if (beta == 0.72) {
        folder_in   = "Resultados_simulacion/MAIN/0.72/EVOLUCION/JOEL";
        ruta_salida   = "Resultados_simulacion/MAIN/0.72/PROMEDIOS_ERRORES/resultados_Jo.txt";
    } else if (beta == 0.8) {
        folder_in   = "Resultados_simulacion/MAIN/0.8/EVOLUCION/JOEL";
        ruta_salida   = "Resultados_simulacion/MAIN/0.8/PROMEDIOS_ERRORES/resultados_Jo.txt";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Na1)
    if (beta == 0.72) {
        folder_in   = "Resultados_simulacion/MAIN/0.72/EVOLUCION/NASIO_1";
        ruta_salida   = "Resultados_simulacion/MAIN/0.72/PROMEDIOS_ERRORES/resultados_Na1.txt";
    } else if (beta == 0.8) {
        folder_in   = "Resultados_simulacion/MAIN/0.8/EVOLUCION/NASIO_1";
        ruta_salida   = "Resultados_simulacion/MAIN/0.8/PROMEDIOS_ERRORES/resultados_Na1.txt";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#elif defined(Jo1)
    if (beta == 0.72) {
        folder_in   = "Resultados_simulacion/MAIN/0.72/EVOLUCION/JOEL_1";
        ruta_salida   = "Resultados_simulacion/MAIN/0.72/PROMEDIOS_ERRORES/resultados_Jo1.txt";
    } else if (beta == 0.8) {
        folder_in   = "Resultados_simulacion/MAIN/0.8/EVOLUCION/JOEL_1";
        ruta_salida   = "Resultados_simulacion/MAIN/0.8/PROMEDIOS_ERRORES/resultados_Jo1.txt";
    } else {
        printf("PON EL VALOR DE BETA QUE TOCA, AMIGO MIO\n");
    }
#else
    #error "Debes definir Na, Ba o Jo antes de compilar, waton culiao";
#endif
    

    DIR *dir = opendir(folder_in);
    if (!dir) {
        perror("No se pudo abrir la carpeta");
        return 1;
    }

    FILE *out = fopen(ruta_salida, "w");
    if (!out) {
        perror("No se pudo crear el fichero de salida");
        closedir(dir);
        return 1;
    }

    struct dirent *entry;
    char ruta[1024];
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "I_", 2) == 0 && strstr(entry->d_name, ".txt")) {
            snprintf(ruta, sizeof(ruta), "%s/%s", folder_in, entry->d_name);
            printf("Procesando %s...\n", ruta);
            procesar_fichero(ruta, out);
        }
    }

    closedir(dir);
    fclose(out);
    printf("Hecho. Resultados guardados en %s\n", ruta_salida);

    return 0;
}
