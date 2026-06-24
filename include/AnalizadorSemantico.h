#ifndef ANALIZADORSEMANTICO_H
#define ANALIZADORSEMANTICO_H

#include <vector>
#include <string>
#include <iostream>
#include "AnalizadorSintactico.h"

using namespace std;

struct Simbolo {
    string nombre;
    int tipo;      // 24 (entier), 25 (reel), 26 (chaine)
    int nivel;
};

struct Ambito {
    vector<Simbolo> simbolos;
};

class AnalizadorSemantico {
    private:
        vector<Ambito> pilaAmbitos;
        vector<string> errores;
        int nivelActual;

        void entrarAmbito();
        void salirAmbito();
        bool existeEnAmbitoActual(const string& nombre);
        Simbolo* buscarSimbolo(const string& nombre);

        int evaluarTipo(NodoS* n);
        string tipoAString(int tipo);
        void procesarNodo(NodoS* n);

    public:
        AnalizadorSemantico();
        void analizar(NodoS* raiz);
        void imprimirErrores();
        const vector<string>& obtErrores() const { return errores; }
};

#endif // ANALIZADORSEMANTICO_H
