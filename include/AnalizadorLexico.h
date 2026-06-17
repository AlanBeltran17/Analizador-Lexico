#ifndef ANALIZADORLEXICO_H
#define ANALIZADORLEXICO_H
#include <stdio.h>

typedef struct {
    FILE *archivo;
    int linea;
} AnaliLexi;

typedef struct {
    int clase;
    char lexema[128];
    int linea;
} Token;

void iniLexi(AnaliLexi *lex, FILE *fp);
Token ObtTok(AnaliLexi *lex);
void impTok(Token t);

#endif // ANALIZADORLEXICO_H
