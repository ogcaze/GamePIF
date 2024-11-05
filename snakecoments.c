#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

// Definindo as dimensões do campo de jogo
#define LARGURA 50
#define ALTURA 20

// Estrutura para representar uma posição no campo
typedef struct {
    int x;
    int y;
} Position;

// Array para armazenar as posições da cobra, tamanho máximo de 100 segmentos
Position snake[100];
int snakeLength = 4;  // Tamanho inicial da cobra
Position fruit;       // Posição da fruta no campo
int direction = 'd';  // Direção inicial da cobra ('d' para direita)

// Função para inicializar o jogo
void initGame() {
    screenInit(1);       // Inicializa a tela (definida em screen.h)
    keyboardInit();      // Inicializa o teclado (definido em keyboard.h)
    screenDrawBorders(); // Desenha as bordas do campo de jogo

    // Posiciona a cobra no centro do campo
    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = LARGURA / 2 - i;
        snake[i].y = ALTURA / 2;
    }

    // Gera uma posição aleatória para a fruta
    srand(time(0));
    fruit.x = rand() % (LARGURA - 2) + 1;
    fruit.y = rand() % (ALTURA - 2) + 1;
}

// Função para desenhar a cobra e a fruta na tela
void drawSnakeAndFruit() {
    screenSetColor(GREEN, BLACK); // Define a cor da cobra
    for (int i = 0; i < snakeLength; i++) {
        screenGotoxy(snake[i].x, snake[i].y);
        printf("O"); // Desenha cada segmento da cobra
    }

    screenSetColor(RED, BLACK);   // Define a cor da fruta
    screenGotoxy(fruit.x, fruit.y);
    printf("X"); // Desenha a fruta
    screenUpdate(); // Atualiza a tela com as novas posições
}

// Função para mover a cobra de acordo com a direção
void moveSnake() {
    // Move o corpo da cobra (cada segmento segue o anterior)
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Atualiza a posição da cabeça com base na direção atual
    if (direction == 'w') snake[0].y--;      // Cima
    else if (direction == 's') snake[0].y++; // Baixo
    else if (direction == 'a') snake[0].x--; // Esquerda
    else if (direction == 'd') snake[0].x++; // Direita
}

// Função para verificar se a cobra colidiu com a fruta
int checkCollision() {
    if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
        snakeLength++; // Aumenta o comprimento da cobra
        // Gera uma nova posição aleatória para a fruta
        fruit.x = rand() % (LARGURA - 2) + 1;
        fruit.y = rand() % (ALTURA - 2) + 1;
        return 1;
    }
    return 0;
}

// Função para verificar se a cobra colidiu com o próprio corpo
int checkSelfCollision() {
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return 1; // Colisão detectada
        }
    }
    return 0;
}

int main() {
    initGame(); // Inicializa o jogo

    while (1) {
        // Verifica se há entrada de direção do teclado
        if (keyboardHit()) {
            char key = keyboardGetchar(); // Obtém a tecla pressionada
            // Atualiza a direção se a tecla é válida e evita a direção oposta
            if ((key == 'w' && direction != 's') || 
                (key == 's' && direction != 'w') ||
                (key == 'a' && direction != 'd') || 
                (key == 'd' && direction != 'a')) {
                direction = key;
            }
        }

        moveSnake(); // Move a cobra na direção atual

        // Verifica colisão com o próprio corpo
        if (checkSelfCollision()) {
            break; // Sai do loop principal em caso de colisão (fim de jogo)
        }

        checkCollision(); // Verifica colisão com a fruta

        screenClear();      // Limpa a tela para redesenhar
        screenDrawBorders(); // Redesenha as bordas
        drawSnakeAndFruit(); // Desenha a cobra e a fruta
        timerDelay(100);     // Controla a velocidade do jogo
    }

    // Mensagem de fim de jogo
    screenClear();
    printf("Game Over! Pontuação: %d\n", snakeLength - 4); // Exibe a pontuação final

    keyboardDestroy(); // Libera recursos de teclado
    screenDestroy();   // Libera recursos de tela
    return 0;
}
