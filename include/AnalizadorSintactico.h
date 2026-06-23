#ifndef ANALIZADORSINTACTICO_H
#define ANALIZADORSINTACTICO_H

#include "AnalizadorLexico.h"
#include <bits/stdc++.h>

using namespace std;

struct NodoS
{
    NodoS *hijo[4];
    NodoS *hermano;
    int tokenClase;
    string nombre;
    string lexema;
    NodoS() {
        for(int i=0;i<4;i++) hijo[i]=nullptr;
        hermano = nullptr;
        tokenClase = -1;
    }
};

#define DECLARACION  31
#define ASIGNACION   32
#define ENTRADA      33
#define SALIDA       34
#define CONDICIONAL  35
#define BUCLE_WHILE  36
#define BUCLE_FOR    37
#define BLOQUE       38
#define INIT         -1

class ArbSintactico
{
    private:
        AnaliLexi lex;
        Token token;
        bool pararErr;
        vector<string> Lierr;

        NodoS* nueNodo(string n);
        NodoS* nueNodoId(string n, int clase, string lex);
        void addHijo(NodoS* p, NodoS* h);
        void addHermano(NodoS* n, NodoS* h);
        void repErr(string mns, int linea);
        void ErrorAper(string nomarch);
        void ErrorDescTok(string esperado, string encontrado, int linea);
        void Errorpuncom(int linea);
        void ErrorIdentificador(int linea);
        void ErrorExpEsp(int linea);
        bool coincidencia(int clasEsp);
        void sigTok();
        void sincro(const set<int>& Tsinc);
        string NomToken(int clase);

        // Funciones de Descenso Recursivo EBNF
        NodoS* prinimp();
        NodoS* sentencia();
        NodoS* declaracion();
        NodoS* asignacion();
        NodoS* entrada_salida();
        NodoS* condicional();
        NodoS* bucle_tantque();
        NodoS* bucle_pour();
        NodoS* codigo();
        NodoS* bool_exp();
        NodoS* incremento();
        NodoS* expre();
        NodoS* termino();
        NodoS* factor();

    public:
        ArbSintactico(bool detPrim = false);
        NodoS* obtRaiz(string nomarch);
        vector<string>& obtErrores();
        void impErrores() ;
        void imp(NodoS* n, int nivel) ;
        static void libArb(NodoS* n);
};
#endif // ANALIZADORSINTACTICO_H
