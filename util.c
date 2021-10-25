#include <stdio.h>
#include <string.h>
#include "util.h"

// Implementacao das funcoes do util.h

int input(char * str, int size) {
    int i = 0;    
    char c = getchar();    
    while (c != '\n') {        
        if (i < size - 1) {            
            str[i] = c;            
            i++;                 
        }        
        c = getchar();   
    }    
    str[i] = '\0';    
    return i;
}

short leia_reg(char * buffer, int tam, FILE* arq){
    short comp_reg;

    if (fread(&comp_reg, sizeof(comp_reg), 1, arq) == 0) {
        return 0;
    }

    if (comp_reg < tam) {
        comp_reg = fread(buffer, sizeof(char), comp_reg, arq);
        buffer[comp_reg] = '\0';
        return comp_reg;
    } else {
        printf("Buffer overflow\n");
        return 0;
    }
}

void concatena_campo(char *buffer, char *campo){
	strcat(buffer, campo); 
    strcat(buffer, DELIM_STR);
}

void limpa_buffer(char * buffer, int tam) {
    int i;
    for (i = 0; i < tam; i++) {
        buffer[i] = '\0';
    }
}

void flag_removido(char * valor_ped, FILE * arq) {
    int i = 0;
    char c;
    
    while ((c = fgetc(arq)) != EOF) {
        if (c == '|') {
            break;
        } else {
            if (c != '*') {
                valor_ped[i] = c;
                i++; 
            }
        }
    }
    valor_ped[i] = '\0';
}