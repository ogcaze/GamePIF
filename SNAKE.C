#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define LARGURA 50
#define ALTURA 20

typedef struct Posicao {
    int x;
    int y;
    struct Posicao *proximo;
} Posicao;

Posicao *cobra = NULL;
int comprimentoCobra = 4;
Posicao fruta;
int direcao = 'd';

void iniciarJogo() {
    screenInit(1);
    keyboardInit();
    screenDrawBorders();

    cobra = (Posicao *)malloc(sizeof(Posicao));
    cobra->x = LARGURA / 2;
    cobra->y = ALTURA / 2;
    cobra->proximo = NULL;

    Posicao *atual = cobra;
    for (int i = 1; i < comprimentoCobra; i++) {
        Posicao *novoSegmento = (Posicao *)malloc(sizeof(Posicao));
        novoSegmento->x = LARGURA / 2 - i;
        novoSegmento->y = ALTURA / 2;
        novoSegmento->proximo = NULL;
        atual->proximo = novoSegmento;
        atual = novoSegmento;
    }

    srand(time(0));
    fruta.x = rand() % (LARGURA - 2) + 1;
    fruta.y = rand() % (ALTURA - 2) + 1;
}

void desenharCobraEFruta() {
    screenSetColor(GREEN, BLACK);
    Posicao *atual = cobra;
    while (atual != NULL) {
        screenGotoxy(atual->x, atual->y);
        printf("O");
        atual = atual->proximo;
    }

    screenSetColor(RED, BLACK);
    screenGotoxy(fruta.x, fruta.y);
    printf("X");
    screenUpdate();
}

void moverCobra() {
    Posicao *novaCabeca = (Posicao *)malloc(sizeof(Posicao));
    novaCabeca->x = cobra->x;
    novaCabeca->y = cobra->y;

    if (direcao == 'w') novaCabeca->y--;      
    else if (direcao == 's') novaCabeca->y++; 
    else if (direcao == 'a') novaCabeca->x--; 
    else if (direcao == 'd') novaCabeca->x++; 

    novaCabeca->proximo = cobra;
    cobra = novaCabeca;

    if (cobra->x != fruta.x || cobra->y != fruta.y) {
        Posicao *atual = cobra;
        while (atual->proximo->proximo != NULL) {
            atual = atual->proximo;
        }
        free(atual->proximo);
        atual->proximo = NULL;
    } else {
        comprimentoCobra++;
        fruta.x = rand() % (LARGURA - 2) + 1;
        fruta.y = rand() % (ALTURA - 2) + 1;
    }
}

int verificarColisao() {
    if (cobra->x <= 0 || cobra->x >= LARGURA - 1 || cobra->y <= 0 || cobra->y >= ALTURA - 1) {
        return 1;
    }
    Posicao *atual = cobra->proximo;
    while (atual != NULL) {
        if (cobra->x == atual->x && cobra->y == atual->y) {
            return 1;
        }
        atual = atual->proximo;
    }
    return 0;
}

void salvarPontuacao(int pontuacao) {
    FILE *arquivo = fopen("top_scores.txt", "a");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo de pontuações.");
        return;
    }
    fprintf(arquivo, "Pontuação: %d\n", pontuacao);
    fclose(arquivo);
}

void exibirTopPontuacoes() {
    FILE *arquivo = fopen("top_scores.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhuma pontuação registrada ainda.\n");
        return;
    }
    printf("Top Pontuações:\n");
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        printf("%s", linha);
    }
    fclose(arquivo);
}

int main() {
    iniciarJogo();

    while (1) {
        if (keyboardHit()) {
            char tecla = keyboardGetchar();
            if ((tecla == 'w' && direcao != 's') || 
                (tecla == 's' && direcao != 'w') ||
                (tecla == 'a' && direcao != 'd') || 
                (tecla == 'd' && direcao != 'a')) {
                direcao = tecla;
            }
        }

        moverCobra();

        if (verificarColisao()) {
            break;
        }

        screenClear();
        screenDrawBorders();
        desenharCobraEFruta();
        timerDelay(100); 
    }

    screenClear();
    printf("Game Over! Pontuação: %d\n", comprimentoCobra - 4);
    salvarPontuacao(comprimentoCobra - 4);
    exibirTopPontuacoes();

    Posicao *atual = cobra;
    while (atual != NULL) {
        Posicao *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }

    keyboardDestroy();
    screenDestroy();
    return 0;
}
