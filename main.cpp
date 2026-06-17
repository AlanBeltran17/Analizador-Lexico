#include <iostream>
#include <stdio.h>
#include "AnalizadorLexico.h"

using namespace std;

int main() {
    AnaliLexi lexico;
    FILE *fp;

    fp = fopen("programa.txt", "r");
    if (!fp) {
        printf("Error: No se pudo abrir el archivo.\n");
        return 1;
    }

    iniLexi(&lexico, fp);

    printf("=======================================\n");
    printf(" CORRIDO DEL ANALIZADOR LEXICO (CodeBleu)\n");
    printf("=======================================\n");
    printf("ID \t LEXEMA \t\t (LINEA)\n");
    printf("---------------------------------------\n");

    while (1) {
        Token t = ObtTok(&lexico);

        impTok(t);

        if (t.clase == 23) {
            printf("---------------------------------------\n");
            printf("Analisis finalizado con exito (EOF).\n");
            break;
        }

        if (t.clase == 0) {
            printf("---------------------------------------\n");
            printf("Analisis detenido por error lexico.\n");
            break;
        }
    }

    fclose(fp);
    return 0;
}
