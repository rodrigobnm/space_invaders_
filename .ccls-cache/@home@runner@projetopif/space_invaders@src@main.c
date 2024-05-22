#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para a função usleep

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position pos;
} Player;

typedef struct {
    Position pos;
    int active;
} Projectile;

typedef struct {
    Position pos;
    int alive;
} Enemy;

#define ENEMIES_PER_ROW 10
#define NUM_ROWS 5
#define NUM_ENEMIES (ENEMIES_PER_ROW * NUM_ROWS)
#define MAX_PROJECTILES 7
#define WIDTH 81
#define HEIGHT 24

// Variável global para armazenar o score
int score = 0;
char playerName[20];

void initialize(Player *player, Enemy enemies[], Projectile playerProjectiles[], Projectile enemyProjectiles[]) {
    player->pos.x = WIDTH / 2;
    player->pos.y = HEIGHT - 3;

    for (int i = 0; i < NUM_ENEMIES; ++i) {
        enemies[i].pos.x = (i % ENEMIES_PER_ROW + 1) * (WIDTH / (ENEMIES_PER_ROW + 1));
        enemies[i].pos.y = 3 + (i / ENEMIES_PER_ROW);
        enemies[i].alive = 1;
    }
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        playerProjectiles[i].active = 0;
        enemyProjectiles[i].active = 0;
    }
}

void draw_game_over() {
    screenInit(1);
    const char *message = "GAME-OVER";
    int message_length = strlen(message);
    int start_x = (WIDTH - message_length) / 2;
    int start_y = HEIGHT / 2;

    for (int i = 0; i < message_length; ++i) {
        draw_char(start_x + i, start_y, message[i]);
    }
  
}

void draw_score() {
    char score_str[40];
    sprintf(score_str, "Player: %s  Score: %d", playerName, score);
    int message_length = strlen(score_str);
    int start_x = (WIDTH - message_length) / 2;

    for (int i = 0; i < message_length; ++i) {
        draw_char(start_x + i, 0, score_str[i]);
    }
}

void desenho(Player player, Enemy enemies[], Projectile playerProjectiles[], Projectile enemyProjectiles[]) {
    screenClear();

    screenInit(1);

    // Desenha o jogador
    draw_char(player.pos.x, player.pos.y, 'W');

    // Desenha inimigos
    for (int i = 0; i < NUM_ENEMIES; ++i) {
        if (enemies[i].alive) {
            draw_char(enemies[i].pos.x, enemies[i].pos.y, 'M');
        }
    }

    // Desenha projéteis
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        if (playerProjectiles[i].active) {
            draw_char(playerProjectiles[i].pos.x, playerProjectiles[i].pos.y, '!');
        }
        if (enemyProjectiles[i].active) {
            draw_char(enemyProjectiles[i].pos.x, enemyProjectiles[i].pos.y, '!');
        }
    }

    // Desenha o score
    draw_score();

    screenUpdate();
}

void atualiza(Player *player, Enemy enemies[], Projectile playerProjectiles[], Projectile enemyProjectiles[]) {
    for (int i = 0; i < MAX_PROJECTILES; ++i) {
        if (playerProjectiles[i].active) {
            playerProjectiles[i].pos.y--;
            if (playerProjectiles[i].pos.y < 1) {
                playerProjectiles[i].active = 0;
            }
            // Verifica colisão com inimigos (projétil subindo)
            for (int j = 0; j < NUM_ENEMIES; ++j) {
                if (enemies[j].alive && playerProjectiles[i].pos.x == enemies[j].pos.x &&
                    playerProjectiles[i].pos.y == enemies[j].pos.y) {
                    enemies[j].alive = 0;
                    playerProjectiles[i].active = 0;
                    score++; // Incrementa o score
                }
            }
        }

        if (enemyProjectiles[i].active) {
            enemyProjectiles[i].pos.y++;
            if (enemyProjectiles[i].pos.y >= HEIGHT - 1) {
                enemyProjectiles[i].active = 0;
            }
            // Verifica colisão com o jogador (projétil descendo)
            if (enemyProjectiles[i].pos.x == player->pos.x && enemyProjectiles[i].pos.y == player->pos.y) {
                screenClear();
                draw_game_over();
                screenUpdate();
                usleep(3000000);
                exit(0);
            }
        }
    }

    for (int i = 0; i < NUM_ENEMIES; ++i) {
        if (enemies[i].alive && rand() % 100 < 5) { // 5% de chance de disparar
            for (int j = 0; j < 3; ++j) { // Lança 3 projéteis
                for (int k = 0; k < MAX_PROJECTILES; ++k) {
                    if (!enemyProjectiles[k].active) {
                        enemyProjectiles[k].pos.x = enemies[i].pos.x;
                        enemyProjectiles[k].pos.y = enemies[i].pos.y + 1;
                        enemyProjectiles[k].active = 1;
                        break;
                    }
                }
            }
        }
    }

    if (keyhit()) {
        int ch = readch();
        if (ch == 'a' && player->pos.x > 1)
            player->pos.x--;
        if (ch == 'd' && player->pos.x < WIDTH - 2)
            player->pos.x++;
        if (ch == ' ') {
            for (int i = 0; i < MAX_PROJECTILES; ++i) {
                if (!playerProjectiles[i].active) {
                    playerProjectiles[i].pos.x = player->pos.x;
                    playerProjectiles[i].pos.y = player->pos.y - 1;
                    playerProjectiles[i].active = 1;
                    break;
                }
            }
        }
    }
}

void acaba() {
    keyboardDestroy();
    timerDestroy();
    screenDestroy();
}

void get_player_name() {
    const char *message = "Digite seu nome: \n";
    int message_length = strlen(message);
    int start_x = (WIDTH - message_length) / 2;
    int start_y = HEIGHT / 2;

    for (int i = 0; i < message_length; ++i) {
        draw_char(start_x + i, start_y, message[i]);
    }
    screenUpdate();

    int index = 0;
    while (1) {
        if (keyhit()) {
            int ch = readch();
            if (ch == '\n' || ch == '\r') {
                playerName[index] = '\0';
                break;
            } else if (ch == '\b' && index > 0) {
                index--;
                playerName[index] = ' ';
                draw_char(start_x + message_length + index, start_y, ' ');
                screenUpdate();
            } else if (index < sizeof(playerName) - 1) {
                playerName[index++] = ch;
                draw_char(start_x + message_length + index - 1, start_y, ch);
                screenUpdate();
            }
        }
    }
}

int main() {
    screenInit(1);
    timerInit(0);
    keyboardInit(0);

    get_player_name();

    Player player;
    Enemy enemies[NUM_ENEMIES];
    Projectile playerProjectiles[MAX_PROJECTILES];
    Projectile enemyProjectiles[MAX_PROJECTILES];

    initialize(&player, enemies, playerProjectiles, enemyProjectiles);

    while (1) {
        desenho(player, enemies, playerProjectiles, enemyProjectiles);
        atualiza(&player, enemies, playerProjectiles, enemyProjectiles);
        usleep(60000);
    }

    acaba();
    return 0;
}
