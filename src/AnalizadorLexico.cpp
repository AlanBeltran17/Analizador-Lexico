#include "AnalizadorLexico.h"
#include <string.h>

int esEsp(char c) { return (c==' ' || c=='\t' || c=='\r' || c=='\n'); }
int esLetra(char c) { return ((c>='a' && c<='z') || (c>='A' && c<='Z')); }
int esDigito(char c) { return (c>='0' && c<='9'); }
int esLOD(char c) { return esLetra(c) || esDigito(c); }

void iniLexi(AnaliLexi *lex, FILE *fp) {
    lex->archivo = fp;
    lex->linea = 1;
}

void veriReser(Token *t) {
    if (t->clase != 1) return;
    if (strcmp(t->lexema, "entier") == 0)   t->clase = 24;
    else if (strcmp(t->lexema, "reel") == 0)    t->clase = 25;
    else if (strcmp(t->lexema, "chaine") == 0)  t->clase = 26;
    else if (strcmp(t->lexema, "si") == 0)      t->clase = 27;
    else if (strcmp(t->lexema, "sinon") == 0)   t->clase = 28;
    else if (strcmp(t->lexema, "tantque") == 0) t->clase = 29;
    else if (strcmp(t->lexema, "pour") == 0)    t->clase = 30;
    else if (strcmp(t->lexema, "afficher") == 0) t->clase = 21;
    else if (strcmp(t->lexema, "lire") == 0)     t->clase = 22;
}

void impTok(Token t) {
    printf("%d \t \"%s\" \t (linea %d)\n", t.clase, t.lexema, t.linea);
}

Token ObtTok(AnaliLexi *lex) {
    Token t; t.lexema[0] = '\0'; t.linea = lex->linea;
    int c;
    while ((c = getc(lex->archivo)) != EOF) {
        if (c == '\n') lex->linea++;
        if (!esEsp(c)) break;
    }
    if (c == EOF) { t.clase = 23; return t; }

    // Logica de Comentarios
    if (c == '/') {
        int s = getc(lex->archivo);
        if (s == '/') {
            while ((c = getc(lex->archivo)) != '\n' && c != EOF);
            return ObtTok(lex);
        } else if (s == '*') {
            int previo = 0;
            while (1) {
                int ch = getc(lex->archivo);
                if (ch == '\n') lex->linea++;
                if (ch == EOF) { t.clase = 0; strcpy(t.lexema, "Error: Comentario no cerrado"); return t; }
                if (previo == '*' && ch == '/') {
                    return ObtTok(lex);
                }
                previo = ch;
            }
        }
        ungetc(s, lex->archivo);
        t.clase = 8; strcpy(t.lexema, "/"); return t;
    }

    // Simbolos
    if (c == '+') { t.clase=5; strcpy(t.lexema, "+"); return t; }
    if (c == '-') { t.clase=6; strcpy(t.lexema, "-"); return t; }
    if (c == '*') { t.clase=7; strcpy(t.lexema, "*"); return t; }
    if (c == '(') { t.clase=9; strcpy(t.lexema, "("); return t; }
    if (c == ')') { t.clase=10; strcpy(t.lexema, ")"); return t; }
    if (c == '{') { t.clase=11; strcpy(t.lexema, "{"); return t; }
    if (c == '}') { t.clase=12; strcpy(t.lexema, "}"); return t; }
    if (c == ';') { t.clase=13; strcpy(t.lexema, ";"); return t; }
    if (c == '"') {
        int i = 0;
        while (1) {
            int ch = getc(lex->archivo);
            if (ch == '"' || ch == EOF) break;
            t.lexema[i++] = ch;
        }
        t.lexema[i] = '\0';
        t.clase = 4;
        return t;
    }

    // Operadores Compuestos
    if (c == '=') {
        int s = getc(lex->archivo);
        if (s == '=') { t.clase=19; strcpy(t.lexema,"=="); }
        else { ungetc(s,lex->archivo); t.clase=18; strcpy(t.lexema,"="); }
        return t;
    }
    // Operadores de comparacion y negacion
    if (c == '<') {
        int s = getc(lex->archivo);
        if (s == '=') { t.clase=16; strcpy(t.lexema,"<="); }
        else if (s == '<') { t.clase=32; strcpy(t.lexema,"<<"); }
        else { ungetc(s,lex->archivo); t.clase=14; strcpy(t.lexema,"<"); }
        return t;
    }

    if (c == '>') {
        int s = getc(lex->archivo);
        if (s == '=') { t.clase=17; strcpy(t.lexema,">="); }
        else if (s == '>') { t.clase=33; strcpy(t.lexema,">>"); }
        else { ungetc(s,lex->archivo); t.clase=15; strcpy(t.lexema,">"); }
        return t;
    }

    if (c == '!') {
        int s = getc(lex->archivo);
        if (s == '=') { t.clase=20; strcpy(t.lexema,"!="); }
        else {
            // Si el caracter no es '=', es un error lexico
            ungetc(s,lex->archivo);
            t.clase=0;
            strcpy(t.lexema,"!");
        }
        return t;
    }

    // Numeros (Enteros y Reales)
    if (esDigito(c)) {
        int i = 0;
        t.lexema[i++] = c;

        while (1) {
            int x = getc(lex->archivo);
            if (esDigito(x)) {
                t.lexema[i++] = x;
            } else if (x == '.') { // Si encontramos un punto, es un numero real
                t.lexema[i++] = x;
                // Ahora leemos los decimales
                while (1) {
                    int d = getc(lex->archivo);
                    if (esDigito(d)) {
                        t.lexema[i++] = d;
                    } else {
                        ungetc(d, lex->archivo);
                        break;
                    }
                }
                t.clase = 3; // ID 3 para REAL
                t.lexema[i] = '\0';
                return t;
            } else {
                ungetc(x, lex->archivo);
                break;
            }
        }
        t.lexema[i] = '\0';
        t.clase = 2; // ID 2 para ENTERO
        return t;
    }

    // Identificadores
    if (esLetra(c)) {
        int i = 0; t.lexema[i++] = c;
        while (1) {
            int x = getc(lex->archivo);
            if (!esLOD(x)) { ungetc(x, lex->archivo); break; }
            t.lexema[i++] = x;
        }
        t.lexema[i] = '\0'; t.clase = 1;
        veriReser(&t);
        return t;
    }

    t.clase = 0; // Error loxico
    t.lexema[0] = c; t.lexema[1] = '\0';
    return t;
}
