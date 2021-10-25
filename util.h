#ifndef UTIL_H
#define UTIL_H

// constantes e macros
#define DELIM_STR "|"
#define DELIM_CH '|'
#define REMOVE_STR "|"
#define  TRUE     1
#define  FALSE    0
#define TAM_MAX_REG 64
#define TAM_STR 30

// Declaracoes funcoes
int input(char *, int);
short leia_reg(char*, int, FILE*); //funcao usada no programa le_registros.c
void concatena_campo(char *, char *);
void limpa_buffer(char *, int );
 void flag_removido(char valor_ped[TAM_STR],FILE *);

#endif