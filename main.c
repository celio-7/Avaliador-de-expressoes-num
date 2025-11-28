#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "expressao.h"

void testar(char *expr) {
    printf("\n=============================================\n");
    printf("Expressão Pós-Fixa: %s\n", expr);

    float valor = getValorPosFixa(expr);
    if (valor != valor) {
        printf("Valor: ERRO (expressão inválida)\n");
    } else {
        printf("Valor: %.6f\n", valor);
    }

    char *infixa = getFormaInFixa(expr);
    if (infixa == NULL) {
        printf("Forma Infixa: ERRO ao converter\n");
    } else {
        printf("Forma Infixa: %s\n", infixa);
    }
}

int main() {

    char entrada[512];

    while (1) {
        printf("\n> ");
        fgets(entrada, sizeof(entrada), stdin);

        entrada[strcspn(entrada, "\n")] = '\0';

        if (strcmp(entrada, "sair") == 0)
            break;

        if (strlen(entrada) < 1)
            continue;

        testar(entrada);
    }

    printf("\nPrograma encerrado.\n");
    return 0;
}
