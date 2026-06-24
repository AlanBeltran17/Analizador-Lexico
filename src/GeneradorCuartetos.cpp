#include "GeneradorCuartetos.h"

/* ================= CONSTRUCTOR ================= */

GeneradorCuartetos::GeneradorCuartetos() {
    tempCont = 0;
}

/* ================= TEMPORALES ================= */

string GeneradorCuartetos::nuevoTemp() {
    tempCont++;
    return "T" + to_string(tempCont);
}

/* ================= GENERACION ================= */

void GeneradorCuartetos::generar(NodoS* raiz) {
    procesarNodo(raiz);
}

/* ================= RECORRIDO GENERAL ================= */

void GeneradorCuartetos::procesarNodo(NodoS* n) {
    if(!n) return;

    /* -------- DECLARACION -------- */
    if(n->tokenClase == DECLARACION) {
        if (n->hijo[0] && n->hijo[1]) {
            string tipo = n->hijo[0]->lexema;
            string izq = n->hijo[1]->lexema;

            // Cuarteto conceptual de declaración
            lista.push_back({tipo, izq, "-", "-"});

            // Si tiene inicialización (ej: entier a = 5;)
            if (n->hijo[2]) {
                string der = generarExpr(n->hijo[2]);
                Cuarteto q;
                q.op = "=";
                q.arg1 = der;
                q.arg2 = "-";
                q.res = izq;
                lista.push_back(q);
            }
        }
    }

    /* -------- ASIGNACION -------- */
    if(n->tokenClase == ASIGNACION) {
        if (n->hijo[0] && n->hijo[1]) {
            string izq = n->hijo[0]->lexema;
            string der = generarExpr(n->hijo[1]);

            Cuarteto q;
            q.op = "=";
            q.arg1 = der;
            q.arg2 = "-";
            q.res = izq;
            lista.push_back(q);
        }
    }

    /* -------- CONDICIONAL (si / sinon) -------- */
    if(n->tokenClase == CONDICIONAL) {
        // En un nivel intermedio simplificado, solo extraemos la condicion
        string cond = generarExpr(n->hijo[0]);
        lista.push_back({"SI", cond, "-", "?"});
    }

    /* -------- BUCLE TANTQUE (mientras) -------- */
    if(n->tokenClase == BUCLE_WHILE) {
        string cond = generarExpr(n->hijo[0]);
        lista.push_back({"TANTQUE", cond, "-", "?"});
    }

    /* -------- BUCLE POUR (para) -------- */
    if(n->tokenClase == BUCLE_FOR) {
        // En el AST de pour, hijo[1] suele ser la condicion booleana
        string cond = generarExpr(n->hijo[1]);
        lista.push_back({"POUR", cond, "-", "?"});
    }

    /* -------- ENTRADA / SALIDA (afficher / lire) -------- */
    if(n->nombre == "entrada_salida") {
        if(n->lexema == "afficher") {
            string expr = generarExpr(n->hijo[0]);
            lista.push_back({"AFFICHER", expr, "-", "-"});
        } else if (n->lexema == "lire") {
            string id = n->hijo[0]->lexema;
            lista.push_back({"LIRE", id, "-", "-"});
        }
    }

    // Recorrido de los hijos en orden
    for(int i = 0; i < 4; i++) {
        procesarNodo(n->hijo[i]);
    }

    // Recorrido de los hermanos
    procesarNodo(n->hermano);
}

/* ================= EXPRESIONES ================= */

string GeneradorCuartetos::generarExpr(NodoS* n) {
    if(!n) return "";

    /* -------- VALOR (Literal o Variable) -------- */
    if(n->nombre == "val") {
        if(n->tokenClase == 1) { // Identificador
            return n->lexema;
        } else { // Literales (entero, real, cadena)
            return n->lexema;
        }
    }

    /* -------- OPERADOR ARITMETICO O LOGICO -------- */
    if(n->nombre == "op" || n->nombre == "bool") {
        string izq = generarExpr(n->hijo[0]);
        string der = generarExpr(n->hijo[1]);
        string t = nuevoTemp();

        lista.push_back({
            n->lexema, // El operador en sí (+, -, *, /, <, >, ==)
            izq,
            der,
            t
        });

        return t;
    }

    /* -------- EXPRESION ENVUELTA (Parentesis) -------- */
    if(n->nombre == "expre") {
        return generarExpr(n->hijo[0]);
    }

    return "";
}

/* ================= IMPRESION ================= */

void GeneradorCuartetos::imprimir() {
    cout << "\n=== CUARTETOS (CodeBlue) ===\n";
    for(size_t i = 0; i < lista.size(); i++) {
        cout << i << ": ( "
             << lista[i].op << ", "
             << lista[i].arg1 << ", "
             << lista[i].arg2 << ", "
             << lista[i].res << " )\n";
    }
}
