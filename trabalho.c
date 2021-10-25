#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void importacao(char *);
int busca_reg(char *, char operacao[], FILE *);
void remove_reg(char *, FILE *);
void inserir_reg(char *, FILE *);
void executa_operacoes(char *);
void imprime_ped();

int main(int argc, char *argv[]) {
    char *nome_arq;

    if (argc == 3 && strcmp(argv[1], "-i") == 0) {
        printf("Modo de importacao ativado ... nome do arquivo = %s\n", argv[2]);
        importacao(argv[2]);
    } else if (argc == 3 && strcmp(argv[1], "-e") == 0) {
        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        executa_operacoes(argv[2]);
    } else if (argc == 2 && strcmp(argv[1], "-p") == 0) {
        printf("Modo de impressao da PED ativado ...\n");
        imprime_ped();
    } else {

        fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i|-e) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    return 0;
}

void importacao(char *arq_read) {
    int i = 0;
    int cabecalho = -1;
    int cont_campo = 0;
    char c;
    char buffer[TAM_MAX_REG];
    FILE *arq;
    FILE *arqDados;

    if((arq = fopen(arq_read, "r"))!= NULL) {
        if((arqDados = fopen("dados.dat", "wb")) != NULL) {
            limpa_buffer(buffer, TAM_MAX_REG);
            fwrite(&cabecalho, sizeof(int), 1, arqDados);
            while ((c=fgetc(arq)) != EOF) {
                buffer[i] = c;
                i++;
                if (c == DELIM_CH) {
                    cont_campo++;
                    if (cont_campo == 4) {
                        fwrite(&buffer, sizeof(char), TAM_MAX_REG, arqDados);
                        limpa_buffer(buffer, TAM_MAX_REG);
                        i = 0;
                        cont_campo = 0;
                        //cabecalho++;
                    }
                }
            }
            //fseek(arqDados,0,SEEK_SET);
            //fwrite(&cabecalho, sizeof(int), 1, arqDados);
        }
    }
}

void executa_operacoes(char *arq) {
    FILE *arquivo;

    if ((arquivo = fopen(arq, "r+b")) == NULL){
        printf("Erro na abertura do arquivo -- programa abortado\n");
        exit(EXIT_FAILURE);
    }

    char operacao[TAM_MAX_REG+2];
    input(operacao, TAM_MAX_REG+2);
    while (operacao[0] == 'b' || operacao[0] == 'r' || operacao[0] == 'i') {
        char *registro = malloc(strlen(operacao));
        if (operacao[0] == 'b' && strlen(operacao) == 8) {
            busca_reg(strncpy(registro, operacao + 2, 7), "b", arquivo);
        } else if (operacao[0] == 'r') {
            remove_reg(strncpy(registro, operacao + 2, 7), arquivo);
        } else if (operacao[0] == 'i') {
            int i;
            char reg[TAM_MAX_REG];
            limpa_buffer(reg, TAM_MAX_REG);
            strcat(reg, strncpy(registro, operacao + 2, strlen(operacao)));
            inserir_reg(reg, arquivo);
        }
        input(operacao, TAM_MAX_REG+2);
    }
    fclose(arquivo);
}

int busca_reg(char *inscricao, char operacao[], FILE *arq) {
    int achou = FALSE;
    char buffer[TAM_MAX_REG];
    char *campo_inscricao;
    char *campo;
    int rrn = 0;

    fseek(arq, 4, SEEK_SET);
    fread(&buffer, sizeof(char), TAM_MAX_REG, arq);
    while (!achou && !feof(arq)) {
        campo_inscricao = strtok(buffer, DELIM_STR);
        if (strcmp(inscricao, campo_inscricao) == 0) {
            achou = TRUE;
        } else {
            fread(&buffer, sizeof(char), TAM_MAX_REG, arq);
            rrn++;
        }
    }

    if (achou) { 
        if (operacao[0] == 'b') {
            printf("Registro encontrado: ");
            printf("%s|", campo_inscricao);
            campo = strtok(NULL, DELIM_STR);
            while (campo != NULL)
            {
                printf("%s|", campo);
                campo = strtok(NULL, DELIM_STR);
            }
            int byte_offset = rrn * 64 + 4;
            printf(" (RRN = %d / byte-offset %d)\n", rrn, byte_offset);
            return rrn;
        } else if (operacao[0] == 'r') {
            return rrn;
        }
    } else {
        if (operacao[0] == 'b') {
            printf("Busca pelo registro de chave \"%s\"\nErro: registro nao encontrado!\n", inscricao);
        } else if (operacao[0] == 'r') {
            printf("Remocao do registro de chave \"%s\"\nErro: registro nao encontrado!\n", inscricao);
        }
        return -1;
    }    
}

void remove_reg(char *inscricao, FILE *arq) {
    int ped = 0;
    char valor_ped[4];
    char buffer_remove[TAM_STR] = {"*"};

    int rrn = busca_reg(inscricao, "r", arq);
    if (rrn != -1) {
        fseek(arq, 0, SEEK_SET);
        fread(&ped, sizeof(int), 1, arq);
        sprintf(valor_ped, "%d", ped);
        int byte_offset = rrn * 64 + 4;
        fseek(arq, byte_offset, SEEK_SET);
        strcat(buffer_remove, valor_ped);
        strcat(buffer_remove, DELIM_STR);//*-1|55|IVAN ORSINI MARTINS|FISICA|1227,4|
        if(fwrite(&buffer_remove, sizeof(char), strlen(buffer_remove), arq) > 0) {
            fseek(arq, 0, SEEK_SET);
            if((fwrite(&rrn, sizeof(int), 1, arq)) > 0) {
                printf("Remocao do registro de chave \"%s\"\nRegistro removido!\nPosicao: RRN = %d (byte-offset %d)\n", inscricao, rrn, byte_offset);
            }
        }
    }

}

void inserir_reg(char *reg, FILE *arq) {
    int ped = 0;
    char valor_ped[TAM_STR];

    fseek(arq, 0, SEEK_SET);
    fread(&ped, sizeof(int), 1, arq);

    if (ped == -1) {
        fseek(arq, 0, SEEK_END);
        if ((fwrite(reg, sizeof(char), TAM_MAX_REG, arq)) > 0) {
            printf("Insercao do registro de chave \"%s\"\nLocal: fim do arquivo\n", strtok(reg, DELIM_STR));
        }
    } else {
        int byte_offset = 64 * ped + 4;
        
        fseek(arq, byte_offset, SEEK_SET);
        flag_removido(valor_ped, arq);
        int topo_ped = atoi(valor_ped);
        printf("%s\n", valor_ped);
        fseek(arq, 0, SEEK_SET);
        if ((fwrite(&topo_ped, sizeof(int), 1, arq)) > 0) {
            fseek(arq, byte_offset, SEEK_SET);
            if ((fwrite(reg, sizeof(char), TAM_MAX_REG, arq)) > 0) {
                printf("Insercao do registro de chave \"%s\"\nLocal: RRN = %d (byte-offset %d) [reutilizado]\n", strtok(reg, DELIM_STR), ped, byte_offset);
            }
        }
    }
}

void imprime_ped() {
    FILE * arquivo;
    int ped = 0;

    if ((arquivo = fopen("dados.dat", "rb")) == NULL){
        printf("Erro na abertura do arquivo -- programa abortado\n");
        exit(EXIT_FAILURE);
    }

    fseek(arquivo, 0, SEEK_SET);
    fread(&ped, sizeof(int), 1, arquivo);
    if (ped == -1) {
        printf("Nao ha espaco fragmentado!\n");
    } else {
        int total = 1;
        char valor_ped[TAM_STR];
        printf("Espaco disponivel no RRN: %d\n", ped);

        while (ped != -1) {
            int byte_offset = ped * TAM_MAX_REG + 4;
            fseek(arquivo, byte_offset, SEEK_SET);
            flag_removido(valor_ped, arquivo);
            ped = atoi(valor_ped);
            
            if (ped != -1) {
                fseek(arquivo, 0, SEEK_SET);
                printf("Espaco disponivel no RRN: %d\n", ped);
                total++;
            }
        }
        printf("Total de espacos disponiveis: %d\n", total);
    }

}

//gcc trabalho.c util.c -o trabalho
//.\trabalho.exe -e dados.dat
//.\trabalho.exe -i candidatos.txt
//.\trabalho.exe -p
//i 215355|IVAN ORSINI MARTINS|FISICA|1227,4|
//i 129874|MILANA BERNARTT|AGRONOMIA|684,3|