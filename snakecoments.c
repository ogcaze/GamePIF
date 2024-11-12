#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define LARGURA 50
#define ALTURA 20

// Estrutura para representar uma posição na tela (x, y) e o próximo segmento da cobra
typedef struct Position {
    int x;
    int y;
    struct Position *next;
} Position;

Position *snake = NULL; // Ponteiro para a cabeça da cobra
int snakeLength = 4; // Comprimento inicial da cobra
Position fruit; // Posição da fruta
int direction = 'd'; // Direção inicial

// Função para inicializar o jogo e a cobra
void initGame() {
    screenInit(1);
    keyboardInit();
    screenDrawBorders();

    // Aloca e posiciona a cabeça da cobra no centro da tela
    snake = (Position *)malloc(sizeof(Position));
    snake->x = LARGURA / 2;
    snake->y = ALTURA / 2;
    snake->next = NULL;

    // Cria o corpo da cobra usando alocação dinâmica
    Position *current = snake;
    for (int i = 1; i < snakeLength; i++) {
        Position *newSegment = (Position *)malloc(sizeof(Position));
        newSegment->x = LARGURA / 2 - i;
        newSegment->y = ALTURA / 2;
        newSegment->next = NULL;
        current->next = newSegment;
        current = newSegment;
    }

    // Posiciona a fruta em um lugar aleatório
    srand(time(0));
    fruit.x = rand() % (LARGURA - 2) + 1;
    fruit.y = rand() % (ALTURA - 2) + 1;
}

// Função para desenhar a cobra e a fruta na tela
void drawSnakeAndFruit() {
    screenSetColor(GREEN, BLACK);
    Position *current = snake;
    while (current != NULL) {
        screenGotoxy(current->x, current->y);
        printf("O");
        current = current->next;
    }

    screenSetColor(RED, BLACK);
    screenGotoxy(fruit.x, fruit.y);
    printf("X");
    screenUpdate();
}

// Função para mover a cobra na direção especificada
void moveSnake() {
    Position *newHead = (Position *)malloc(sizeof(Position));
    newHead->x = snake->x;
    newHead->y = snake->y;

    if (direction == 'w') newHead->y--;      
    else if (direction == 's') newHead->y++; 
    else if (direction == 'a') newHead->x--; 
    else if (direction == 'd') newHead->x++; 

    newHead->next = snake;
    snake = newHead;

    if (snake->x != fruit.x || snake->y != fruit.y) {
        Position *current = snake;
        while (current->next->next != NULL) {
            current = current->next;
        }
        free(current->next);
        current->next = NULL;
    } else {
        snakeLength++;
        fruit.x = rand() % (LARGURA - 2) + 1;
        fruit.y = rand() % (ALTURA - 2) + 1;
    }
}

// Função para verificar colisão com a parede e com o corpo da cobra
int checkCollision() {
    if (snake->x <= 0 || snake->x >= LARGURA - 1 || snake->y <= 0 || snake->y >= ALTURA - 1) {
        return 1;
    }
    Position *current = snake->next;
    while (current != NULL) {
        if (snake->x == current->x && snake->y == current->y) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Função para salvar a pontuação no arquivo top_scores.txt 
void saveScore(int score) {
    FILE *file = fopen("top_scores.txt", "a");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de pontuações.");
        return;
    }
    fprintf(file, "Pontuação: %d\n", score);
    fclose(file);
}

// Função para exibir as pontuações salvas
void displayTopScores() {
    FILE *file = fopen("top_scores.txt", "r");
    if (file == NULL) {
        printf("Nenhuma pontuação registrada ainda.\n");
        return;
    }
    printf("Top Scores:\n");
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    fclose(file);
}

int main() {
    initGame();

    while (1) {
        if (keyboardHit()) {
            char key = keyboardGetchar();
            if ((key == 'w' && direction != 's') || 
                (key == 's' && direction != 'w') ||
                (key == 'a' && direction != 'd') || 
                (key == 'd' && direction != 'a')) {
                direction = key;
            }
        }

        moveSnake();

        if (checkCollision()) {
            break;
        }

        screenClear();
        screenDrawBorders();
        drawSnakeAndFruit();
        timerDelay(100); 
    }

    screenClear();
    printf("Game Over! Pontuação: %d\n", snakeLength - 4);
    saveScore(snakeLength - 4);
    displayTopScores();

    Position *current = snake;
    while (current != NULL) {
        Position *next = current->next;
        free(current);
        current = next;
    }

    keyboardDestroy();
    screenDestroy();
    return 0;
}
