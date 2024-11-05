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
int snakeLength = 4;
Position fruit;
int direction = 'd';

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

void moveSnake() {
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    if (direction == 'w') snake[0].y--;      
    else if (direction == 's') snake[0].y++; 
    else if (direction == 'a') snake[0].x--; 
    else if (direction == 'd') snake[0].x++; 
}

int checkCollision() {
    if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
        snakeLength++;
        fruit.x = rand() % (LARGURA - 2) + 1;
        fruit.y = rand() % (ALTURA - 2) + 1;
        return 1;
    }
    return 0;
}

int checkSelfCollision() {
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return 1;
        }
    }
    return 0;
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

        if (checkSelfCollision()) {
            break;
        }

        checkCollision();

        screenClear();
        screenDrawBorders();
        drawSnakeAndFruit();
        timerDelay(100); 
    }

    screenClear();
    printf("Game Over! Pontuação: %d\n", snakeLength - 4);

    keyboardDestroy();
    screenDestroy();
    return 0;
}
