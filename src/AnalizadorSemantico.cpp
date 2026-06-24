#include "AnalizadorSemantico.h"

/* ================= CONSTRUCTOR ================= */

AnalizadorSemantico::AnalizadorSemantico() {
    nivelActual = -1;
    entrarAmbito();
}

/* ================= MANEJO DE ÁMBITOS ================= */

void AnalizadorSemantico::entrarAmbito() {
    Ambito nuevoAmbito;
    pilaAmbitos.push_back(nuevoAmbito);
    nivelActual++;
}

void AnalizadorSemantico::salirAmbito() {
    if (!pilaAmbitos.empty()) {
        pilaAmbitos.pop_back();
        nivelActual--;
    }
}

/* ================= BUSQUEDA DE SÍMBOLOS ================= */

Simbolo* AnalizadorSemantico::buscarSimbolo(const string& nombre) {
    // Busca desde el ambito mas interno (local) hacia el mas externo (global)
    for (int i = pilaAmbitos.size() - 1; i >= 0; i--) {
        for (size_t j = 0; j < pilaAmbitos[i].simbolos.size(); j++) {
            if (pilaAmbitos[i].simbolos[j].nombre == nombre)
                return &pilaAmbitos[i].simbolos[j];
        }
    }
    return nullptr;
}

bool AnalizadorSemantico::existeEnAmbitoActual(const string& nombre) {
    if (pilaAmbitos.empty()) return false;
    const Ambito &a = pilaAmbitos.back();
    for (size_t i = 0; i < a.simbolos.size(); i++) {
        if (a.simbolos[i].nombre == nombre)
            return true;
    }
    return false;
}

/* ================= AUXILIARES ================= */

string AnalizadorSemantico::tipoAString(int tipo) {
    switch(tipo) {
        case 24: return "entier";
        case 25: return "reel";
        case 26: return "chaine";
        default: return "desconocido";
    }
}

/* ================= EVALUACION DE TIPOS ================= */

int AnalizadorSemantico::evaluarTipo(NodoS* n) {
    if (!n) return -1;

    // Nodo de Valor Constante o Variable
    if (n->nombre == "val") {
        if (n->tokenClase == 2) return 24; // ENTERO literal -> entier
        if (n->tokenClase == 3) return 25; // REAL literal -> reel
        if (n->tokenClase == 4) return 26; // CADENA literal -> chaine

        if (n->tokenClase == 1) { // Identificador (Variable)
            Simbolo* s = buscarSimbolo(n->lexema);
            if (!s) {
                errores.push_back("ERROR SEMANTICO: Uso de variable no declarada '" + n->lexema + "'");
                return -1;
            }
            return s->tipo;
        }
    }

    // Operaciones Aritmeticas (+, -, *, /)
    if (n->nombre == "op") {
        int t1 = evaluarTipo(n->hijo[0]);
        int t2 = evaluarTipo(n->hijo[1]);
        if (t1 == -1 || t2 == -1) return -1;

        if (t1 != t2) {
            errores.push_back("ERROR SEMANTICO: Operacion entre tipos incompatibles (" + tipoAString(t1) + " y " + tipoAString(t2) + ")");
            return -1;
        }
        return t1;
    }

    if (n->nombre == "expre") {
        return evaluarTipo(n->hijo[0]);
    }

    return -1;
}

/* ================= ANALISIS RECURSIVO ================= */

void AnalizadorSemantico::analizar(NodoS* raiz) {
    procesarNodo(raiz);
}

void AnalizadorSemantico::procesarNodo(NodoS* n) {
    if (!n) return;

    bool ambitoCreado = false;

    // Manejo de Ambitos Estructurados (Bloques de código y ciclos)
    if (n->tokenClase == BUCLE_FOR || n->nombre == "codigo" || n->nombre == "sinon_bloque") {
        entrarAmbito();
        ambitoCreado = true;
    }

    /* -------- DECLARACION DE VARIABLES -------- */
    if (n->tokenClase == DECLARACION) {
        if (n->hijo[0] && n->hijo[1]) {
            int tipo = n->hijo[0]->tokenClase;
            string nombre = n->hijo[1]->lexema;

            if (existeEnAmbitoActual(nombre)) {
                errores.push_back("ERROR SEMANTICO: Doble declaracion de la variable '" + nombre + "' en el mismo ambito.");
            } else {
                Simbolo s;
                s.nombre = nombre;
                s.tipo = tipo;
                s.nivel = nivelActual;
                pilaAmbitos.back().simbolos.push_back(s);
            }

            if (n->hijo[2]) {
                int tDer = evaluarTipo(n->hijo[2]);
                if (tDer != -1 && tipo != tDer) {
                    errores.push_back("ERROR SEMANTICO: Tipo incompatible en la inicializacion de '" + nombre + "'. Se esperaba " + tipoAString(tipo) + " pero se obtuvo " + tipoAString(tDer) + ".");
                }
            }
        }
    }

    /* -------- ASIGNACION -------- */
    if (n->tokenClase == ASIGNACION) {
        if (n->hijo[0] && n->hijo[1]) {
            string nombre = n->hijo[0]->lexema;
            Simbolo* s = buscarSimbolo(nombre);

            if (!s) {
                errores.push_back("ERROR SEMANTICO: Asignacion a variable no declarada '" + nombre + "'.");
            } else {
                int tIzq = s->tipo;
                int tDer = evaluarTipo(n->hijo[1]);
                if (tIzq != -1 && tDer != -1 && tIzq != tDer) {
                    errores.push_back("ERROR SEMANTICO: Tipos incompatibles en asignacion de '" + nombre + "'. No se puede asignar " + tipoAString(tDer) + " a " + tipoAString(tIzq) + ".");
                }
            }
        }
    }

    /* -------- RECORRIDO DE LOS HIJOS -------- */
    // Primero procesamos los hijos para asegurar que las declaraciones internas (como en el 'pour') se registren primero
    for (int i = 0; i < 4; i++) {
        procesarNodo(n->hijo[i]);
    }

    /* -------- VALIDACION DE EXPRESIONES RELACIONALES (si, tantque, pour) -------- */
    // Ahora que los hijos ya se procesaron, evaluamos la condicion de manera segura
    if (n->tokenClase == CONDICIONAL || n->tokenClase == BUCLE_WHILE || n->tokenClase == BUCLE_FOR) {
        NodoS* boolNode = (n->tokenClase == BUCLE_FOR) ? n->hijo[1] : n->hijo[0];

        if (boolNode && boolNode->nombre == "bool") {
            int t1 = evaluarTipo(boolNode->hijo[0]);
            int t2 = evaluarTipo(boolNode->hijo[2]);
            if (t1 != -1 && t2 != -1 && t1 != t2) {
                errores.push_back("ERROR SEMANTICO: Comparacion entre tipos incompatibles en estructura de control (" + tipoAString(t1) + " y " + tipoAString(t2) + ").");
            }
        }
    }

    // Salida del Ambito si fue creado para este nodo
    if (ambitoCreado) {
        salirAmbito();
    }

    /* -------- RECORRIDO DE HERMANOS -------- */
    procesarNodo(n->hermano);
}

/* ================= REPORTE DE ERRORES ================= */

void AnalizadorSemantico::imprimirErrores() {
    if (errores.empty()) {
        cout << "\n>>> [OK] Analisis semantico completado sin errores en CodeBlue.\n";
        return;
    }

    cout << "\n=== ERRORES SEMANTICOS (CodeBlue) ===\n";
    for (size_t i = 0; i < errores.size(); i++) {
        cout << errores[i] << endl;
    }
}
