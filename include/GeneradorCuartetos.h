#ifndef GENERADORCUARTETOS_H
#define GENERADORCUARTETOS_H

#include <vector>
#include <string>
#include <iostream>
#include "AnalizadorSintactico.h"

using namespace std;

struct Cuarteto {
    string op;
    string arg1;
    string arg2;
    string res;
};

class GeneradorCuartetos {
    private:
        vector<Cuarteto> lista;
        int tempCont;

        string nuevoTemp();
        string generarExpr(NodoS* n);
        void procesarNodo(NodoS* n);

    public:
        GeneradorCuartetos();

        void generar(NodoS* raiz);
        void imprimir();
        const vector<Cuarteto>& obtLista() const { return lista; }
};

#endif // GENERADORCUARTETOS_H
