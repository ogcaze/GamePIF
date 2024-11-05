#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define LARGURA 50
#define ALTURA 20

typedef struct {
    int x;
    int y;
} Position;

Position snake[100];
int snakeLength = 5;
Position fruit;

void initGame() {
    screenInit(1);
    keyboardInit();
    
    screenDrawBorders();

    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = LARGURA / 2 - i;
        snake[i].y = ALTURA / 2;
    }

    srand(time(0));
    fruit.x = rand() % (LARGURA - 2) + 1;
    fruit.y = rand() % (ALTURA - 2) + 1;
}

void drawSnakeAndFruit() {
    screenSetColor(GREEN, BLACK);
    for (int i = 0; i < snakeLength; i++) {
        screenGotoxy(snake[i].x, snake[i].y);
        printf("O");
    }

    screenSetColor(RED, BLACK);
    screenGotoxy(fruit.x, fruit.y);
    printf("X");
    screenUpdate();
}

int main() {
    initGame();
    drawSnakeAndFruit();

    getchar();

    keyboardDestroy();
    screenDestroy();
    return 0;
}
