#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

// Definindo o tamanho do campo de jogo
#define LARGURA 50
#define ALTURA 20

// Estrutura para representar posição
typedef struct {
    int x;
    int y;
} Position;

Position snake[100];  // Array para armazenar a posição do corpo da cobra
int snakeLength = 4;  // Tamanho inicial da cobra
Position fruit;       // Posição da fruta

void initGame() {
    // Inicializa a tela e configura o teclado
    screenInit(1);
    keyboardInit();
    
    // Desenha as bordas do campo de jogo
    screenDrawBorders();

    // Configura a posição inicial da cobra no meio do campo
    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = LARGURA / 2 - i;
        snake[i].y = ALTURA / 2;
    }

    // Coloca a primeira fruta em uma posição aleatória
    srand(time(0));
    fruit.x = rand() % (LARGURA - 2) + 1;
    fruit.y = rand() % (ALTURA - 2) + 1;
}

void drawSnakeAndFruit() {
    // Desenha a cobra
    screenSetColor(GREEN, BLACK);
    for (int i = 0; i < snakeLength; i++) {
        screenGotoxy(snake[i].x, snake[i].y);
        printf("O");
    }

    // Desenha a fruta
    screenSetColor(RED, BLACK);
    screenGotoxy(fruit.x, fruit.y);
    printf("X");
    screenUpdate();
}

int main() {
    initGame();
    drawSnakeAndFruit();

    // Pausa para que possamos ver o campo inicial
    getchar();  // Pressione Enter para sair

    // Limpa a tela e restaura configurações iniciais
    keyboardDestroy();
    screenDestroy();
    return 0;
}
