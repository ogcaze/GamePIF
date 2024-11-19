#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

// Definições de largura e altura da área de jogo
#define LARGURA 50
#define ALTURA 20

//Pontuacão que vai pro arquivo
typedef struct{
    char nome[4];
    int pontuacao;
}Pontuacao;

// Estrutura para posicionar elementos no jogo
typedef struct {
    int x;
    int y;
} Posicao;

// Estrutura para um segmento da cobra
typedef struct SegmentoCobra {
    Posicao pos;                  // Posição do segmento
    struct SegmentoCobra *proximo;// Ponteiro para o próximo segmento
} SegmentoCobra;

// Estrutura para a cobra
typedef struct {
    SegmentoCobra *cabeca;        // Ponteiro para a cabeça da cobra
    SegmentoCobra *rabo;          // Ponteiro para o rabo da cobra
    int comprimento;              // Comprimento atual da cobra
} Cobra;

// Estrutura para o estado do jogo
typedef struct {
    Cobra cobra;                  // Cobra no jogo
    Posicao comida;               // Posição da comida
    int pontuacao;                // Pontuação atual do jogador
    int fimDeJogo;                // Flag para indicar o fim do jogo
} EstadoJogo;

// Renderizar a comida na tela
void maca(int mx, int my) {
    screenSetColor(RED, BLACK);
    screenGotoxy(mx, my);
    printf("❦");
}

// Exibir a pontuação na tela
void pontuacao(int pontos) {
    screenSetColor(BLUE, BLACK);
    screenGotoxy(LARGURA + 5, 5);
    printf("Pontos: %d", pontos);
}

// Atualizar e renderizar o estado atual do jogo na tela
void render(EstadoJogo *jogo) {
    screenClear();

    for (int y = 0; y < ALTURA; y++) {
        for (int x = 0; x < LARGURA; x++) {
            // Renderiza as bordas do jogo
            if (x == 0 || x == LARGURA - 1 || y == 0 || y == ALTURA - 1){
                screenSetColor(DARKGRAY, DARKGRAY);
                printf("█");
            }
            else if (x == jogo->comida.x && y == jogo->comida.y){
                maca(x, y); // Mostra a maçã
            }
            else {
                SegmentoCobra *segmento = jogo->cobra.cabeca;
                int segmentoPrintado = 0;
                // Mostrando a cobra
                while (segmento != NULL) {
                    if (segmento->pos.x == x && segmento->pos.y == y) {
                        screenSetColor(GREEN, BLACK);
                        printf("◼︎");
                        segmentoPrintado = 1;
                        break;
                    }
                    segmento = segmento->proximo;
                }
                // Espaços em branco onde não há cobra nem borda
                if (!segmentoPrintado)
                    printf(" ");
            }
        }
        printf("\n");
    }

    // Exibe a pontuação do jogador
    pontuacao(jogo->pontuacao);
    // Atualiza a tela
    screenUpdate();
}

// Inicializar o estado do jogo
void inicializaJogo(EstadoJogo *jogo) { //////////////
    SegmentoCobra *segmentoInicial = (SegmentoCobra *)malloc(sizeof(SegmentoCobra));

    //Posição inicial da cobra no centro da tela
    segmentoInicial->pos.x = LARGURA / 2;
    segmentoInicial->pos.y = ALTURA / 2;
    segmentoInicial->proximo = NULL;

    // Inicializa a cobra com um segmento
    jogo->cobra.cabeca = segmentoInicial;
    jogo->cobra.rabo = segmentoInicial;
    jogo->cobra.comprimento = 1;

    // Posição aleatória para a comida
    srand(time(0));
    jogo->comida.x = rand() % (LARGURA - 2) + 1;
    jogo->comida.y = rand() % (ALTURA - 2) + 1;

    jogo->pontuacao = 0;
    jogo->fimDeJogo = 0;
}

// Andar com a cobra
void entrada(int *dx, int *dy) {
    if (keyhit()) {
        int tecla = readch();
        switch (tecla) {
            case 'w': *dx = 0; *dy = -1; break; // Move para cima
            case 's': *dx = 0; *dy = 1; break;  // Move para baixo
            case 'a': *dx = -1.99; *dy = 0; break; // Move para a esquerda
            case 'd': *dx = 1.99; *dy = 0; break;  // Move para a direita
        }
    }
}

// Atualizar a posição da cobra
void atualizaCobra(EstadoJogo *jogo, int dx, int dy) { /////////////////////
    SegmentoCobra *novoSegmento = (SegmentoCobra *)malloc(sizeof(SegmentoCobra));
    novoSegmento->pos.x = jogo->cobra.cabeca->pos.x + dx;
    novoSegmento->pos.y = jogo->cobra.cabeca->pos.y + dy;
    novoSegmento->proximo = jogo->cobra.cabeca;

    // Atualiza a cabeça da cobra para o novo segmento
    jogo->cobra.cabeca = novoSegmento;

    // Se a cobra comeu a comida
    if (novoSegmento->pos.x == jogo->comida.x && novoSegmento->pos.y == jogo->comida.y) {
        jogo->pontuacao++;
        // Gera uma nova posição para a comida
        jogo->comida.x = rand() % (LARGURA - 2) + 1;
        jogo->comida.y = rand() % (ALTURA - 2) + 1;
        jogo->cobra.comprimento++;
    }
    else {
        // Move o rabo para a próxima posição, reduzindo o tamanho da cobra
        SegmentoCobra *temp = jogo->cobra.cabeca;
        while (temp->proximo != jogo->cobra.rabo) {
            temp = temp->proximo;
        }
        free(jogo->cobra.rabo);
        jogo->cobra.rabo = temp;
        jogo->cobra.rabo->proximo = NULL;
    }
}

// Verificar se há colisão com parede ou com a cobra
int verificaColisao(EstadoJogo *jogo) { //////////////////////
    SegmentoCobra *temp = jogo->cobra.cabeca->proximo;
    // Verifica colisão da cobra consigo mesma
    while (temp != NULL) {
        if (jogo->cobra.cabeca->pos.x == temp->pos.x && jogo->cobra.cabeca->pos.y == temp->pos.y) {
            return 1;
        }
        temp = temp->proximo;
    }

    // Verifica colisão com as bordas
    if (jogo->cobra.cabeca->pos.x <= 0 || jogo->cobra.cabeca->pos.x >= LARGURA - 1 ||
        jogo->cobra.cabeca->pos.y <= 0 || jogo->cobra.cabeca->pos.y >= ALTURA - 1) {
        return 1;
    }

    return 0;
}

// Função para encerrar o jogo e mostrar a pontuação final
void endGame(EstadoJogo *jogo){
    screenClear();
    printf("GAME OVER!\n");
    printf("PONTUACAO FINAL: %d\n", jogo->pontuacao);
    screenUpdate();
}

// Wait
void espera(int milissegundos) {
    timerUpdateTimer(milissegundos);
    while (!timerTimeOver()) {
        // Espera até que o temporizador expire
    }
}

// Salva pontuação no arquivo
void salvaPontuacao(int pontos, char *nome){
    FILE *arquivo = fopen ("top_scores.txt", "a");
    if (arquivo == NULL){
        printf("Erro ao abrir arquivo!\n");
        return;
    }
    fprintf(arquivo, "%s: %dpts\n", nome, pontos);
    fclose(arquivo);
}

// Recebe os dados do jogador
void dadosjogador(EstadoJogo *jogo){
    char nomeJogador[4];
    printf("Digite seu nome (3 letras): ");
    scanf("%3s", nomeJogador);
    salvaPontuacao(jogo->pontuacao, nomeJogador);
}

// Main
int main() {
    EstadoJogo jogo;
    inicializaJogo(&jogo);

    int dx = 1, dy = 0;

    screenInit(0);
    keyboardInit();
    timerInit(100);

    while (!jogo.fimDeJogo) {
        entrada(&dx, &dy);
        atualizaCobra(&jogo, dx, dy);
        render(&jogo);
        espera(100);
        jogo.fimDeJogo = verificaColisao(&jogo);
    }

    keyboardDestroy();
    screenDestroy();
    timerDestroy();

    endGame(&jogo);
    printf("\n");
    dadosjogador(&jogo);
    printf("\nPontuacao salva em top_scores.txt\n\n\n\n");
    
    return 0;
}