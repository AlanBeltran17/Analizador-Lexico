#include <iostream>
#include <stdio.h>
#include "AnalizadorLexico.h"
#include "AnalizadorSintactico.h"

using namespace std;

void ejecutarLexico(char* nomarch) {
    AnaliLexi lexico;
    FILE *fp = fopen(nomarch, "r");

    if (!fp) {
        printf("Error: No se pudo abrir el archivo 'programa.txt'.\n");
        return;
    }

    iniLexi(&lexico, fp);

    printf("\n=======================================\n");
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
}

void ejecutarSintactico(char* nomarch) {
    printf("\n===========================================\n");
    printf(" CORRIDO DEL ANALIZADOR SINTACTICO (CodeBleu)\n");
    printf("===========================================\n");

    ArbSintactico Sintactico(false);
    NodoS* raiz = Sintactico.obtRaiz(nomarch);

    Sintactico.impErrores();

    if(raiz) {
        printf("\n--- ARBOL SINTACTICO ---\n");
        Sintactico.imp(raiz, 0);

        ArbSintactico::libArb(raiz);
    } else {
        printf("No se pudo generar el arbol sintactico debido a errores graves.\n");
    }
}

int main() {
    int opcion;
    char nombreArchivo[100];
    do {
        printf("\n============= MENU PRINCIPAL =============\n");
        printf("1. Ejecutar Analizador Lexico\n");
        printf("2. Ejecutar Analizador Sintactico\n");
        printf("3. Salir\n");
        printf("==========================================\n");
        printf("Escribe el nombre del archivo: ");
        cin>> nombreArchivo;
        printf("==========================================\n");
        printf("Selecciona una opcion: ");

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(10000, '\n');
            opcion = -1;
        }

        switch (opcion) {
            case 1:
                ejecutarLexico(nombreArchivo);
                break;
            case 2:
                ejecutarSintactico(nombreArchivo);
                break;
            case 3:
                printf("\nSaliendo del compilador...\n");
                break;
            default:
                printf("\nOpcion no valida. Intenta de nuevo.\n");
        }
    } while (opcion != 3);

    return 0;
}
