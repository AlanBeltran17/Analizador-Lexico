#include "AnalizadorSintactico.h"
#include <bits/stdc++.h>

using namespace std;

ArbSintactico::ArbSintactico(bool detPrim) {
    pararErr = detPrim;
    Lierr.clear();
    token.clase = 23;
}

NodoS* ArbSintactico::nueNodo(string n) {
    NodoS* x = new NodoS();
    for(int i=0;i<4;i++) x->hijo[i]=nullptr;
    x->hermano=nullptr;
    x->tokenClase = INIT;
    x->nombre=n;
    return x;
}

NodoS* ArbSintactico::nueNodoId(string n, int clase, string lex) {
    NodoS* x = nueNodo(n);
    x->tokenClase = clase;
    x->lexema=lex;
    return x;
}

void ArbSintactico::addHijo(NodoS* p, NodoS* h) {
    if(!p || !h) return;
    for(int i=0;i<4;i++){
        if(p->hijo[i]==NULL){
            p->hijo[i]=h;
            return;
        }
    }
    if(p->hijo[3]) addHermano(p->hijo[3], h);
}

void ArbSintactico::addHermano(NodoS* n, NodoS* h) {
    if(!n || !h) return;
    while(n->hermano) n=n->hermano;
    n->hermano = h;
}

void ArbSintactico::repErr(string mns, int linea) {
    string esc = (linea > 0) ? "ERROR SINTACTICO EN LA LINEA: " + to_string(linea) + " " + mns : "ERROR SINTACTICO: " + mns;
    Lierr.push_back(esc);
    if(pararErr) { cout << esc << endl; exit(1); }
}

void ArbSintactico::ErrorAper(string nomarch) { repErr("no se pudo abrir el archivo " + nomarch, 0); }
void ArbSintactico::ErrorDescTok(string esperado, string encontrado, int linea) {
    string b = "Token inesperado: se esperaba " + esperado;
    if(!encontrado.empty()) b += " pero se encontro: " + encontrado;
    repErr(b, linea);
}
void ArbSintactico::Errorpuncom(int linea) { repErr("Falta punto y coma ';'", linea); }
void ArbSintactico::ErrorIdentificador(int linea) { repErr("Se esperaba un identificador (id)", linea); }
void ArbSintactico::ErrorExpEsp(int linea) { repErr("Se esperaba una expresion", linea); }

// Mapeo de Tokens
string ArbSintactico::NomToken(int clase) {
    switch(clase) {
        case 1: return "ID"; case 2: return "ENTERO"; case 3: return "REAL";
        case 4: return "CADENA";
        case 5: return "+"; case 6: return "-"; case 7: return "*"; case 8: return "/";
        case 9: return "("; case 10: return ")"; case 11: return "{"; case 12: return "}"; case 13: return ";";
        case 14: return "<"; case 15: return ">"; case 16: return "<="; case 17: return ">=";
        case 18: return "="; case 19: return "=="; case 20: return "!=";
        case 21: return "afficher"; case 22: return "lire";
        case 23: return "EOF";
        case 24: return "entier"; case 25: return "reel"; case 26: return "chaine";
        case 27: return "si"; case 28: return "sinon"; case 29: return "tantque"; case 30: return "pour";
        case 32: return "<<"; case 33: return ">>";
        default: return "DESCONOCIDO";
    }
}

void ArbSintactico::sigTok() { token = ObtTok(&lex); }

bool ArbSintactico::coincidencia(int clasEsp) {
    if(token.clase == clasEsp) {
        sigTok();
        return true;
    } else {
        ErrorDescTok(NomToken(clasEsp), token.lexema, token.linea);
        return false;
    }
}

void ArbSintactico::sincro(const set<int>& Tsinc) {
    while(token.clase != 23 && Tsinc.count(token.clase) == 0) {
        sigTok();
    }
}

NodoS* ArbSintactico::obtRaiz(string nomarch) {
    FILE* fp = fopen(nomarch.c_str(), "r");
    if(!fp) { ErrorAper(nomarch); return nullptr; }
    iniLexi(&lex, fp);
    token = ObtTok(&lex);
    return prinimp();
}

NodoS* ArbSintactico::prinimp() {
    NodoS* raiz = nueNodo("principal");
    while(token.clase != 23) { // 23 es EOF
        NodoS* s = sentencia();
        if(s) {
            addHijo(raiz, s);
        } else {
            sincro({13, 12, 23}); // Busca ';' '}' o EOF para retomar
            if(token.clase == 13) sigTok();
        }
    }
    return raiz;
}

NodoS* ArbSintactico::sentencia() {
    // Manejo seguro
    if(token.clase == 13) { sigTok(); return nullptr; } // Si encontramos un ; extra, lo ignoramos

    if(token.clase >= 24 && token.clase <= 26) return declaracion();
    if(token.clase == 1) return asignacion();
    if(token.clase == 21 || token.clase == 22) return entrada_salida();
    if(token.clase == 27) return condicional();
    if(token.clase == 29) return bucle_tantque();
    if(token.clase == 30) return bucle_pour();

    repErr("Sentencia no reconocida cerca de: " + string(token.lexema), token.linea);
    sigTok();
    return nullptr;
}

NodoS* ArbSintactico::declaracion() {
    NodoS* decl = nueNodo("declaracion");
    decl->tokenClase = DECLARACION;
    addHijo(decl, nueNodoId("tipo", token.clase, token.lexema));
    sigTok();

    if(token.clase == 1) {
        addHijo(decl, nueNodoId("id", 1, token.lexema));
        sigTok();
    } else { ErrorIdentificador(token.linea); }

    if(token.clase == 18) {
        sigTok();
        addHijo(decl, expre());
    }
    coincidencia(13); // ';'
    return decl;
}

NodoS* ArbSintactico::asignacion() {
    NodoS* asig = nueNodo("asignacion");
    asig->tokenClase = ASIGNACION;
    addHijo(asig, nueNodoId("id", 1, token.lexema));
    sigTok();
    coincidencia(18); // '='
    addHijo(asig, expre());
    coincidencia(13); // ';'
    return asig;
}

NodoS* ArbSintactico::entrada_salida() {
    NodoS* io = nueNodo("io");
    if(token.clase == 21) { // afficher
        io->tokenClase = SALIDA; io->nombre = "salida";
        sigTok();
        coincidencia(32); // '<<'
        if(token.clase == 4) { // CADENA
            addHijo(io, nueNodoId("cadena", 4, token.lexema));
            sigTok();
        } else {
            addHijo(io, expre());
        }
    } else if(token.clase == 22) { // lire
        io->tokenClase = ENTRADA; io->nombre = "entrada";
        sigTok();
        coincidencia(33); // '>>'
        if(token.clase == 1) {
            addHijo(io, nueNodoId("id", 1, token.lexema));
            sigTok();
        } else { ErrorIdentificador(token.linea); }
    }
    coincidencia(13); // ';'
    return io;
}

NodoS* ArbSintactico::codigo() {
    NodoS* c = nueNodo("codigo");
    while(token.clase != 12 && token.clase != 23) { // '}' o EOF
        NodoS* s = sentencia();
        if(s) addHijo(c, s);
        else { sincro({13, 12, 23}); if(token.clase == 13) sigTok(); }
    }
    return c;
}

NodoS* ArbSintactico::condicional() {
    NodoS* s = nueNodo("condicional");
    s->tokenClase = CONDICIONAL;
    coincidencia(27); // si
    coincidencia(9);  // '('
    addHijo(s, bool_exp());
    coincidencia(10); // ')'
    coincidencia(11); // '{'
    addHijo(s, codigo());
    coincidencia(12); // '}'

    if(token.clase == 28) { // sinon
        sigTok();
        coincidencia(11); // '{'
        NodoS* c_sinon = nueNodo("sinon_bloque");
        addHijo(c_sinon, codigo());
        addHijo(s, c_sinon);
        coincidencia(12); // '}'
    }
    return s;
}

NodoS* ArbSintactico::bucle_tantque() {
    NodoS* t = nueNodo("tantque");
    t->tokenClase = BUCLE_WHILE;
    coincidencia(29); // tantque
    coincidencia(9);  // '('
    addHijo(t, bool_exp());
    coincidencia(10); // ')'
    coincidencia(11); // '{'
    addHijo(t, codigo());
    coincidencia(12); // '}'
    return t;
}

NodoS* ArbSintactico::incremento() {
    NodoS* inc = nueNodo("incremento");
    if(token.clase == 1) {
        addHijo(inc, nueNodoId("id", 1, token.lexema));
        sigTok();
        if(token.lexema == "++") { // 28 es sinon, verifica lexema o añade token
             sigTok();
        } else if(token.clase == 18) { // '='
            sigTok();
            addHijo(inc, expre());
        }
    }
    return inc;
}

NodoS* ArbSintactico::bucle_pour() {
    NodoS* p = nueNodo("pour");
    p->tokenClase = BUCLE_FOR;
    coincidencia(30); // pour
    coincidencia(9);  // '('
    if(token.clase >= 24 && token.clase <= 26) addHijo(p, declaracion());
    else if(token.clase == 1) addHijo(p, asignacion());
    else coincidencia(13);

    addHijo(p, bool_exp());
    coincidencia(13); // ';'
    addHijo(p, incremento());
    coincidencia(10); // ')'
    coincidencia(11); // '{'
    addHijo(p, codigo());
    coincidencia(12); // '}'
    return p;
}

NodoS* ArbSintactico::bool_exp() {
    NodoS* b = nueNodo("bool");
    addHijo(b, expre());
    // Operadores: < 14, > 15, <= 16, >= 17, == 19, != 20
    if (token.clase >= 14 && token.clase <= 20 && token.clase != 18) {
        addHijo(b, nueNodoId("rel", token.clase, token.lexema));
        sigTok();
    } else { repErr("Operador relacional esperado", token.linea); }
    addHijo(b, expre());
    return b;
}

// expresion = termino { ( "+" | "-" ) termino } ;
NodoS* ArbSintactico::expre() {
    NodoS* nodoIzq = termino();
    while(token.clase == 5 || token.clase == 6) { // '+' o '-'
        NodoS* op = nueNodoId("op", token.clase, token.lexema);
        sigTok();
        addHijo(op, nodoIzq);
        addHijo(op, termino());
        nodoIzq = op;
    }
    return nodoIzq;
}

// termino = factor { ( "*" | "/" ) factor } ;
NodoS* ArbSintactico::termino() {
    NodoS* nodoIzq = factor();
    while(token.clase == 7 || token.clase == 8) { // '*' o '/'
        NodoS* op = nueNodoId("op", token.clase, token.lexema);
        sigTok();
        addHijo(op, nodoIzq);
        addHijo(op, factor());
        nodoIzq = op;
    }
    return nodoIzq;
}

// factor = identificador | numero | "(" expresion ")" ;
NodoS* ArbSintactico::factor() {
    NodoS* f = nullptr;
    // Agregamos la clase 3 (REAL) aquí
    if(token.clase == 1 || token.clase == 2 || token.clase == 3) {
        f = nueNodoId("val", token.clase, token.lexema);
        sigTok();
    } else if(token.clase == 9) { // '('
        sigTok();
        f = expre();
        coincidencia(10); // ')'
    } else {
        ErrorExpEsp(token.linea);
        sigTok();
    }
    return f;
}

vector<string>& ArbSintactico::obtErrores() { return Lierr; }

void ArbSintactico::impErrores() {
    if(Lierr.empty()) return;
    cout << "=== ERRORES SINTACTICOS ===\n";
    for(const auto& err : Lierr) cout << err << "\n";
}

void ArbSintactico::imp(NodoS* n, int nivel) {
    if(!n) return;
    for(int i=0;i<nivel;i++) printf("  ");
    printf("%s", n->nombre.c_str());
    if(n->tokenClase != -1 && !n->lexema.empty()) printf(" (%s)", n->lexema.c_str());
    printf("\n");
    for(int i=0;i<4;i++) imp(n->hijo[i], nivel+1);
    imp(n->hermano, nivel);
}

void ArbSintactico::libArb(NodoS* n) {
    if(!n) return;
    for(int i=0;i<4;i++) libArb(n->hijo[i]);
    libArb(n->hermano);
    delete n;
}
