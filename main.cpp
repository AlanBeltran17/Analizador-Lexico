#include <iostream>
#include <stdio.h>
#include "AnalizadorLexico.h"
#include "AnalizadorSintactico.h"
#include "AnalizadorSemantico.h"
#include "GeneradorCuartetos.h"

using namespace std;

void ejecutarLexico(char* nomarch) {
    AnaliLexi lexico;
    FILE *fp = fopen(nomarch, "r");

    if (!fp) {
        printf("Error: No se pudo abrir el archivo '%s'.\n", nomarch);
        return;
    }

    iniLexi(&lexico, fp);

    printf("\n=======================================\n");
    printf(" CORRIDO DEL ANALIZADOR LEXICO (CodeBlue)\n");
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
    printf(" CORRIDO DEL ANALIZADOR SINTACTICO (CodeBlue)\n");
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

void ejecutarSemantico(char* nomarch) {
    printf("\n===========================================\n");
    printf(" CORRIDO DEL ANALIZADOR SEMANTICO (CodeBlue)\n");
    printf("===========================================\n");

    ArbSintactico Sintactico(false);
    // Primero necesitamos construir el árbol sintáctico
    NodoS* raiz = Sintactico.obtRaiz(nomarch);

    // Si el archivo tiene errores sintácticos críticos, los mostramos y frenamos
    Sintactico.impErrores();

    if (raiz) {
        AnalizadorSemantico Semantico;

        // Ejecutamos la validación de ámbitos y tipos
        Semantico.analizar(raiz);
        Semantico.imprimirErrores();

        // Liberamos el árbol de la memoria ram al terminar
        ArbSintactico::libArb(raiz);
    } else {
        printf("No se pudo realizar el analisis semantico debido a errores sintacticos previos.\n");
    }
}

void ejecutarCuartetos(char* nomarch) {
    printf("\n===========================================\n");
    printf(" GENERACION DE CUARTETOS (CodeBlue)\n");
    printf("===========================================\n");

    ArbSintactico Sintactico(false);
    NodoS* raiz = Sintactico.obtRaiz(nomarch);

    if (raiz) {
        // 1. Verificación Semántica previa
        AnalizadorSemantico Semantico;
        Semantico.analizar(raiz);

        if (Semantico.obtErrores().empty()) {
            printf("Analisis semantico correcto. Generando cuartetos...\n");

            // 2. Generación
            GeneradorCuartetos Gen;
            Gen.generar(raiz);
            Gen.imprimir();
        } else {
            printf("Error: No se pueden generar cuartetos porque existen errores semanticos:\n");
            Semantico.imprimirErrores();
        }

        ArbSintactico::libArb(raiz);
    } else {
        printf("Error: No se pudo generar el arbol sintactico.\n");
    }
}

int main() {
    int opcion;
    char nombreArchivo[100];
    printf("===============================================\n");
    printf("Escribe el nombre del archivo: ");
    cin >> nombreArchivo;
    printf("\n===============================================");
    do {
        printf("\n============= MENU PRINCIPAL =============\n");
        printf("1. Ejecutar Analizador Lexico\n");
        printf("2. Ejecutar Analizador Sintactico\n");
        printf("3. Ejecutar Analizador Semantico\n");
        printf("4. Generar Cuartetos\n");
        printf("5. Cambiar el archivo\n");
        printf("0. Salir\n");
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
                ejecutarSemantico(nombreArchivo);
                break;
            case 4:
                ejecutarCuartetos(nombreArchivo);
                break;
            case 5:
                system("cls");
                printf("===============================================\n");
                printf("Escribe el nombre del archivo: ");
                cin >> nombreArchivo;
                printf("\n===============================================");
                break;
            case 0:
                printf("\nSaliendo del compilador...\n");
                break;
            default:
                printf("\nOpcion no valida. Intenta de nuevo.\n");
        }
    } while (opcion != 0);

    return 0;
}
