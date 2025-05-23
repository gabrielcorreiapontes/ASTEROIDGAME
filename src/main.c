#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "screen.h"
#include "keyboard.h"
#include "timer.h"

#define HIGHSCORE_FILE "highscores.txt"
#define MAX_PLAYERS 5
#define NAVE_VELOCIDADE 2
#define ASTEROID_SPAWN_RATE 30
#define ASTEROID_TAMANHO 3

typedef struct {
    int x, y;
} Nave;

typedef struct Asteroid {
    int x, y;
    int velocidade;
    struct Asteroid* next;
} Asteroid;

typedef struct Tiro {
    int x, y;
    int ativo;
    struct Tiro* next;
} Tiro;

typedef struct {
    char nome[20];
    int pontos;
} Jogador;

typedef enum { MENU, JOGANDO, GAME_OVER, HIGHSCORES } EstadoJogo;

Nave nave;
Asteroid* listaAsteroids = NULL;
Tiro* listaTiros = NULL;
int pontos = 0;
int vidas = 3;
int nivel = 1;
int contadorAsteroids = 0;
int spawnTimer = 0;
EstadoJogo estadoJogo = MENU;
int menuSelecao = 0;
Jogador jogadores[MAX_PLAYERS];
int numJogadores = 0;
char nomeJogador[20] = "";
int nomeIndice = 0;
int matriz[MAX_PLAYERS][2];
char caminhoArquivo[100] = "";

void desenharBordas() {
    screenBoxEnable();
    for (int i = MINX; i <= MAXX; i++) {
        screenGotoxy(i, MINY); printf("%c", BOX_HLINE);
        screenGotoxy(i, MAXY); printf("%c", BOX_HLINE);
    }
    for (int i = MINY; i <= MAXY; i++) {
        screenGotoxy(MINX, i); printf("%c", BOX_VLINE);
        screenGotoxy(MAXX, i); printf("%c", BOX_VLINE);
    }
    screenGotoxy(MINX, MINY); printf("%c", BOX_UPLEFT);
    screenGotoxy(MAXX, MINY); printf("%c", BOX_UPRIGHT);
    screenGotoxy(MINX, MAXY); printf("%c", BOX_DWNLEFT);
    screenGotoxy(MAXX, MAXY); printf("%c", BOX_DWNRIGHT);
    screenBoxDisable();
}

void desenharNave() {
    screenSetColor(GREEN, BLACK);
    screenGotoxy(nave.x, nave.y);
    printf("A");
}

void desenharAsteroid(int x, int y) {
    screenSetColor(BROWN, BLACK);
    for (int i = 0; i < ASTEROID_TAMANHO; i++) {
        for (int j = 0; j < ASTEROID_TAMANHO; j++) {
            if (x+j > MINX && x+j < MAXX && y+i > MINY && y+i < MAXY) {
                screenGotoxy(x+j, y+i);
                printf("*");
            }
        }
    }
}

void limparAsteroid(int x, int y) {
    for (int i = 0; i < ASTEROID_TAMANHO; i++) {
        for (int j = 0; j < ASTEROID_TAMANHO; j++) {
            if (x+j > MINX && x+j < MAXX && y+i > MINY && y+i < MAXY) {
                screenGotoxy(x+j, y+i);
                printf(" ");
            }
        }
    }
}

void desenharTiro(int x, int y) {
    screenSetColor(YELLOW, BLACK);
    screenGotoxy(x, y);
    printf("|");
}

void desenharHUD() {
    screenSetColor(WHITE, BLACK);
    screenGotoxy(MINX+2, MINY);
    printf("Pontos: %d", pontos);
    screenGotoxy(MAXX-15, MINY);
    printf("Vidas: %d | Nivel: %d", vidas, nivel);
}

Asteroid* criarAsteroid(int x, int y, int velocidade) {
    Asteroid* asteroid = (Asteroid*)malloc(sizeof(Asteroid));
    if (asteroid == NULL) exit(1);
    asteroid->x = x;
    asteroid->y = y;
    asteroid->velocidade = velocidade;
    asteroid->next = NULL;
    return asteroid;
}

void adicionarAsteroid(Asteroid** head, Asteroid* novoAsteroid) {
    if (*head == NULL) {
        *head = novoAsteroid;
    } else {
        Asteroid* atual = *head;
        while (atual->next != NULL) atual = atual->next;
        atual->next = novoAsteroid;
    }
    contadorAsteroids++;
}

void removerAsteroid(Asteroid** head, Asteroid* asteroid) {
    if (*head == NULL) return;
    if (*head == asteroid) {
        *head = (*head)->next;
        free(asteroid);
        contadorAsteroids--;
        return;
    }
    Asteroid* atual = *head;
    while (atual->next != NULL && atual->next != asteroid) atual = atual->next;
    if (atual->next == asteroid) {
        atual->next = asteroid->next;
        free(asteroid);
        contadorAsteroids--;
    }
}

Tiro* criarTiro(int x, int y) {
    Tiro* tiro = (Tiro*)malloc(sizeof(Tiro));
    if (tiro == NULL) exit(1);
    tiro->x = x;
    tiro->y = y;
    tiro->ativo = 1;
    tiro->next = NULL;
    return tiro;
}

void adicionarTiro(Tiro** head, Tiro* novoTiro) {
    if (*head == NULL) {
        *head = novoTiro;
    } else {
        Tiro* atual = *head;
        while (atual->next != NULL) atual = atual->next;
        atual->next = novoTiro;
    }
}

void removerTiro(Tiro** head, Tiro* tiro) {
    if (*head == NULL) return;
    if (*head == tiro) {
        *head = (*head)->next;
        free(tiro);
        return;
    }
    Tiro* atual = *head;
    while (atual->next != NULL && atual->next != tiro) atual = atual->next;
    if (atual->next == tiro) {
        atual->next = tiro->next;
        free(tiro);
    }
}

void inicializarMatrizHighScores() {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        matriz[i][0] = i;
        matriz[i][1] = 0;
    }
}

void atualizarMatrizHighScores() {
    for (int i = 0; i < numJogadores; i++) {
        matriz[i][0] = i;
        matriz[i][1] = jogadores[i].pontos;
    }
}

void ordenarMatrizHighScores() {
    for (int i = 0; i < numJogadores - 1; i++) {
        for (int j = 0; j < numJogadores - i - 1; j++) {
            if (matriz[j][1] < matriz[j + 1][1]) {
                int tempIndice = matriz[j][0];
                int tempPontos = matriz[j][1];
                matriz[j][0] = matriz[j + 1][0];
                matriz[j][1] = matriz[j + 1][1];
                matriz[j + 1][0] = tempIndice;
                matriz[j + 1][1] = tempPontos;
                Jogador tempJogador = jogadores[matriz[j][0]];
                jogadores[matriz[j][0]] = jogadores[matriz[j + 1][0]];
                jogadores[matriz[j + 1][0]] = tempJogador;
            }
        }
    }
}

void carregarHighScores() {
    numJogadores = 0;
    inicializarMatrizHighScores();
    FILE* arquivo = fopen(HIGHSCORE_FILE, "r");
    if (arquivo == NULL) {
        return;
    }
    while (numJogadores < MAX_PLAYERS &&
           fscanf(arquivo, "%s %d", jogadores[numJogadores].nome, &jogadores[numJogadores].pontos) == 2) {
        numJogadores++;
    }
    fclose(arquivo);
    atualizarMatrizHighScores();
}

void salvarHighScores() {
    FILE* arquivo = fopen(HIGHSCORE_FILE, "w");
    if (arquivo == NULL) {
        return;
    }
    for (int i = 0; i < numJogadores; i++) {
        fprintf(arquivo, "%s %d\n", jogadores[i].nome, jogadores[i].pontos);
    }
    fclose(arquivo);
}

void adicionarHighScore(const char* nome, int pontos) {
    if (numJogadores < MAX_PLAYERS || pontos > jogadores[numJogadores-1].pontos) {
        if (numJogadores < MAX_PLAYERS) {
            strcpy(jogadores[numJogadores].nome, nome);
            jogadores[numJogadores].pontos = pontos;
            numJogadores++;
        } else {
            strcpy(jogadores[MAX_PLAYERS-1].nome, nome);
            jogadores[MAX_PLAYERS-1].pontos = pontos;
        }
        atualizarMatrizHighScores();
        ordenarMatrizHighScores();
        salvarHighScores();
    }
}

void inicializarJogo() {
    nave.x = MAXX / 2;
    nave.y = MAXY - 2;
    while (listaAsteroids != NULL) {
        Asteroid* temp = listaAsteroids;
        listaAsteroids = listaAsteroids->next;
        free(temp);
    }
    while (listaTiros != NULL) {
        Tiro* temp = listaTiros;
        listaTiros = listaTiros->next;
        free(temp);
    }
    contadorAsteroids = 0;
    spawnTimer = 0;
    pontos = 0;
    vidas = 3;
    nivel = 1;
}

void gerarAsteroid() {
    int x = MINX + 1 + rand() % (MAXX - MINX - ASTEROID_TAMANHO - 1);
    int y = MINY + 1;
    int velocidade = 1;
    Asteroid* asteroid = criarAsteroid(x, y, velocidade);
    adicionarAsteroid(&listaAsteroids, asteroid);
}

void atualizarJogo() {
    screenClear();
    desenharBordas();
    spawnTimer++;
    if (spawnTimer >= ASTEROID_SPAWN_RATE) {
        gerarAsteroid();
        spawnTimer = 0;
    }
    Asteroid* asteroid = listaAsteroids;
    Asteroid* proximoAsteroid;
    while (asteroid != NULL) {
        proximoAsteroid = asteroid->next;
        limparAsteroid(asteroid->x, asteroid->y);
        static int contador = 0;
        contador++;
        if (contador >= 10 - asteroid->velocidade) {
            asteroid->y += 1;
            contador = 0;
        }
        if (asteroid->y + ASTEROID_TAMANHO > MAXY) {
            vidas--;
            removerAsteroid(&listaAsteroids, asteroid);
            if (vidas <= 0) {
                estadoJogo = GAME_OVER;
            }
        }
        else if (asteroid->y + ASTEROID_TAMANHO > nave.y &&
                 asteroid->x <= nave.x && asteroid->x + ASTEROID_TAMANHO > nave.x) {
            vidas--;
            removerAsteroid(&listaAsteroids, asteroid);
            if (vidas <= 0) {
                estadoJogo = GAME_OVER;
            }
        }
        else {
            desenharAsteroid(asteroid->x, asteroid->y);
        }
        asteroid = proximoAsteroid;
    }
    Tiro* tiro = listaTiros;
    Tiro* proximoTiro;
    while (tiro != NULL) {
        proximoTiro = tiro->next;
        if (tiro->ativo) {
            screenGotoxy(tiro->x, tiro->y);
            printf(" ");
            tiro->y--;
            if (tiro->y <= MINY) {
                tiro->ativo = 0;
                removerTiro(&listaTiros, tiro);
            }
            else {
                int colisao = 0;
                asteroid = listaAsteroids;
                while (asteroid != NULL && !colisao) {
                    proximoAsteroid = asteroid->next;
                    if (tiro->x >= asteroid->x && tiro->x < asteroid->x + ASTEROID_TAMANHO &&
                        tiro->y >= asteroid->y && tiro->y < asteroid->y + ASTEROID_TAMANHO) {
                        colisao = 1;
                        tiro->ativo = 0;
                        pontos += 100;
                        if (pontos % 500 == 0 && nivel < 9) {
                            nivel++;
                        }
                        removerAsteroid(&listaAsteroids, asteroid);
                    }
                    asteroid = proximoAsteroid;
                }
                if (colisao) {
                    removerTiro(&listaTiros, tiro);
                } else {
                    desenharTiro(tiro->x, tiro->y);
                }
            }
        }
        tiro = proximoTiro;
    }
    desenharNave();
    desenharHUD();
}

void desenharMenu() {
    screenClear();
    desenharBordas();
    screenSetColor(CYAN, BLACK);
    screenGotoxy(MAXX/2 - 10, MINY+5);
    printf("*** ASTEROIDS ***");
    screenSetColor(WHITE, BLACK);
    screenGotoxy(MAXX/2 - 8, MINY+8);
    printf("%s Novo Jogo", menuSelecao == 0 ? ">" : " ");
    screenGotoxy(MAXX/2 - 8, MINY+10);
    printf("%s High Scores", menuSelecao == 1 ? ">" : " ");
    screenGotoxy(MAXX/2 - 8, MINY+12);
    printf("%s Sair", menuSelecao == 2 ? ">" : " ");
    screenGotoxy(MAXX/2 - 15, MAXY-3);
    printf("Use as setas para selecionar");
    screenGotoxy(MAXX/2 - 10, MAXY-2);
    printf("Enter para confirmar");
}

void desenharGameOver() {
    screenClear();
    desenharBordas();
    screenSetColor(RED, BLACK);
    screenGotoxy(MAXX/2 - 5, MAXY/2 - 2);
    printf("GAME OVER");
    screenSetColor(WHITE, BLACK);
    screenGotoxy(MAXX/2 - 10, MAXY/2);
    printf("Pontuacao: %d", pontos);
    screenGotoxy(MAXX/2 - 15, MAXY/2 + 2);
    printf("Digite seu nome: %s_", nomeJogador);
    screenGotoxy(MAXX/2 - 15, MAXY-3);
    printf("Enter para confirmar");
}

void desenharHighScores() {
    screenClear();
    desenharBordas();
    screenSetColor(YELLOW, BLACK);
    screenGotoxy(MAXX/2 - 10, MINY+3);
    printf("*** HIGH SCORES ***");
    screenSetColor(WHITE, BLACK);
    if (numJogadores == 0) {
        screenGotoxy(MAXX/2 - 15, MINY+8);
        printf("Nenhuma pontuacao registrada ainda!");
    } else {
        for (int i = 0; i < numJogadores; i++) {
            int indiceJogador = matriz[i][0];
            int pontosJogador = matriz[i][1];
            screenGotoxy(MAXX/2 - 10, MINY+6+i*2);
            printf("%d. %-15s %5d", i+1, jogadores[indiceJogador].nome, pontosJogador);
        }
    }
    screenGotoxy(MAXX/2 - 15, MAXY-3);
    printf("Pressione ESC para voltar");
}

int main() {
    srand(time(NULL));
    screenInit(1);
    keyboardInit();
    timerInit(100);
    inicializarMatrizHighScores();
    carregarHighScores();
    int rodando = 1;
    int ch = 0;
    while (rodando) {
        if (keyhit()) {
            ch = readch();
            switch (estadoJogo) {
                case MENU:
                    if (ch == 27) {
                        rodando = 0;
                    } else if (ch == 10 || ch == 13) {
                        if (menuSelecao == 0) {
                            inicializarJogo();
                            estadoJogo = JOGANDO;
                        } else if (menuSelecao == 1) {
                            estadoJogo = HIGHSCORES;
                        } else if (menuSelecao == 2) {
                            rodando = 0;
                        }
                    } else if (ch == 65 || ch == 'w') {
                        menuSelecao = (menuSelecao + 2) % 3;
                    } else if (ch == 66 || ch == 's') {
                        menuSelecao = (menuSelecao + 1) % 3;
                    }
                    break;
                case JOGANDO:
                    if (ch == 27) {
                        estadoJogo = MENU;
                    } else if (ch == 'a' || ch == 68) {
                        if (nave.x > MINX+1) {
                            nave.x -= NAVE_VELOCIDADE;
                            if (nave.x < MINX+1) nave.x = MINX+1;
                        }
                    } else if (ch == 'd' || ch == 67) {
                        if (nave.x < MAXX-1) {
                            nave.x += NAVE_VELOCIDADE;
                            if (nave.x > MAXX-1) nave.x = MAXX-1;
                        }
                    } else if (ch == ' ') {
                        Tiro* tiro = criarTiro(nave.x, nave.y - 1);
                        adicionarTiro(&listaTiros, tiro);
                    }
                    break;
                case GAME_OVER:
                    if (ch == 10 || ch == 13) {
                        if (nomeIndice > 0) {
                            nomeJogador[nomeIndice] = '\0';
                            adicionarHighScore(nomeJogador, pontos);
                            nomeIndice = 0;
                            nomeJogador[0] = '\0';
                            estadoJogo = MENU;
                        }
                    } else if (ch == 8 || ch == 127) {
                        if (nomeIndice > 0) {
                            nomeIndice--;
                            nomeJogador[nomeIndice] = '\0';
                        }
                    } else if (ch == 27) {
                        nomeIndice = 0;
                        nomeJogador[0] = '\0';
                        estadoJogo = MENU;
                    } else if (nomeIndice < 19 && ch >= 32 && ch <= 126) {
                        nomeJogador[nomeIndice++] = ch;
                        nomeJogador[nomeIndice] = '\0';
                    }
                    break;
                case HIGHSCORES:
                    if (ch == 27) {
                        estadoJogo = MENU;
                    }
                    break;
            }
        }
        if (timerTimeOver()) {
            switch (estadoJogo) {
                case MENU:
                    desenharMenu();
                    break;
                case JOGANDO:
                    atualizarJogo();
                    break;
                case GAME_OVER:
                    desenharGameOver();
                    break;
                case HIGHSCORES:
                    desenharHighScores();
                    break;
            }
            screenUpdate();
        }
    }
    keyboardDestroy();
    screenDestroy();
    timerDestroy();
    while (listaAsteroids != NULL) {
        Asteroid* temp = listaAsteroids;
        listaAsteroids = listaAsteroids->next;
        free(temp);
    }
    while (listaTiros != NULL) {
        Tiro* temp = listaTiros;
        listaTiros = listaTiros->next;
        free(temp);
    }
    return 0;
}
