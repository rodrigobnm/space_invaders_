#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

#define ENEMIES_PER_ROW 6
#define NUM_ROWS 4
#define NUM_ENEMIES (ENEMIES_PER_ROW * NUM_ROWS)
#define MAX_PROJECTILES 2
#define WIDTH 81
#define HEIGHT 24

int score = 0;
char playerName[20];
int enemyMoveTimer = 0;
int enemyProjectileMoveCounter = 0;
int enemyDirection = 1;
time_t lastEnemyMoveTime = 0;

void initialize(Player **player, Enemy **enemies,
                Projectile **playerProjectiles, Projectile **enemyProjectiles) {
  *player = (Player *)malloc(sizeof(Player));
  (*player)->pos.x = WIDTH / 2;
  (*player)->pos.y = HEIGHT - 3;

  *enemies = (Enemy *)malloc(NUM_ENEMIES * sizeof(Enemy));
  for (int i = 0; i < NUM_ENEMIES; ++i) {
    (*enemies)[i].pos.x =
        (i % ENEMIES_PER_ROW + 1) * (WIDTH / (ENEMIES_PER_ROW + 1));
    (*enemies)[i].pos.y = 3 + (i / ENEMIES_PER_ROW);
    (*enemies)[i].alive = 1;
  }

  *playerProjectiles =
      (Projectile *)malloc(MAX_PROJECTILES * sizeof(Projectile));
  *enemyProjectiles =
      (Projectile *)malloc(MAX_PROJECTILES * sizeof(Projectile));

  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    (*playerProjectiles)[i].active = 0;
    (*enemyProjectiles)[i].active = 0;
  }
  lastEnemyMoveTime = time(NULL);
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

void draw_victory_message() {
  screenInit(1);
  const char *message = "VOCE VENCEU!";
  int message_length = strlen(message);
  int start_x = (WIDTH - message_length) / 2;
  int start_y = HEIGHT / 2;

  for (int i = 0; i < message_length; ++i) {
    draw_char(start_x + i, start_y, message[i]);
  }
}

void draw_score() {
  char score_str[40];
  sprintf(score_str, "Score: %d", score);
  int message_length = strlen(score_str);
  int start_x = (WIDTH - message_length) / 2;

  for (int i = 0; i < message_length; ++i) {
    draw_char(start_x + i, 0, score_str[i]);
  }
}

void desenho(Player player, Enemy enemies[], Projectile playerProjectiles[],
             Projectile enemyProjectiles[]) {
  screenClear();
  screenInit(1);

  draw_char(player.pos.x, player.pos.y, 'W');

  for (int i = 0; i < NUM_ENEMIES; ++i) {
    if (enemies[i].alive) {
      draw_char(enemies[i].pos.x, enemies[i].pos.y, 'M');
    }
  }

  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    if (playerProjectiles[i].active) {
      draw_char(playerProjectiles[i].pos.x, playerProjectiles[i].pos.y, '!');
    }
    if (enemyProjectiles[i].active) {
      draw_char(enemyProjectiles[i].pos.x, enemyProjectiles[i].pos.y, '!');
    }
  }

  draw_score();
  screenUpdate();
}

void save_score() {
  FILE *file = fopen("scores.txt", "a");
  if (file) {
    fprintf(file, "Player: %s, Score: %d\n", playerName, score);
    fclose(file);
  } else {
    printf("Erro ao abrir o arquivo para salvar o score.\n");
  }
}

void atualiza(Player *player, Enemy enemies[], Projectile playerProjectiles[],
              Projectile enemyProjectiles[]) {

  int allEnemiesDead =
      1; // Variável para verificar se todos os inimigos estão mortos

  for (int i = 0; i < MAX_PROJECTILES; ++i) {
    if (playerProjectiles[i].active) {
      playerProjectiles[i].pos.y--;
      if (playerProjectiles[i].pos.y < 1) {
        playerProjectiles[i].active = 0;
      }

      for (int j = 0; j < NUM_ENEMIES; ++j) {
        if (enemies[j].alive &&
            playerProjectiles[i].pos.x == enemies[j].pos.x &&
            playerProjectiles[i].pos.y == enemies[j].pos.y) {
          enemies[j].alive = 0;
          playerProjectiles[i].active = 0;
          score = score + 10;
        }
      }
    }

    if (enemyProjectiles[i].active) {
      if (enemyProjectileMoveCounter % 3 == 0) {
        enemyProjectiles[i].pos.y++;
        if (enemyProjectiles[i].pos.y >= HEIGHT - 1) {
          enemyProjectiles[i].active = 0;
        }

        if (enemyProjectiles[i].pos.x == player->pos.x &&
            enemyProjectiles[i].pos.y == player->pos.y) {
          screenClear();
          draw_game_over();
          screenUpdate();
          usleep(3000000);
          save_score();
          exit(0);
        }
      }
    }
  }

  enemyProjectileMoveCounter++;

  for (int i = 0; i < NUM_ENEMIES; ++i) {
    if (enemies[i].alive) {
      allEnemiesDead = 0; // Se encontrar um inimigo vivo, define como 0
      if (rand() % 100 < 5) {
        for (int j = 0; j < 3; ++j) {
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
  }

  if (allEnemiesDead) {
    screenClear();
    draw_victory_message();
    screenUpdate();
    usleep(3000000);
    save_score();
    exit(0);
  }

  if (keyhit()) {
    int ch = readch();
    if (ch == 'a' && player->pos.x > 2) {
      player->pos.x--;
    }
    if (ch == 'd' && player->pos.x < WIDTH - 3) {
      player->pos.x++;
    }
    if (ch == ' ') {
      int canShoot = 1;
      for (int i = 0; i < MAX_PROJECTILES; ++i) {
        if (playerProjectiles[i].active) {
          canShoot = 0;
          break;
        }
      }
      if (canShoot) {
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
}

void acaba() {
  keyboardDestroy();
  timerDestroy();
  screenDestroy();
}

void get_player_name() {
  const char *message = "Digite seu nome: ";
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
      } else if (ch == 127 &&
                 index > 0) { // Backspace (127 is ASCII for backspace)
        index--;
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

void view_ranking() {
  while (1) {
    screenClear();
    screenInit(1);
    FILE *file = fopen("scores.txt", "r");
    if (file) {
      char line[100];
      int y = 2;
      while (fgets(line, sizeof(line), file)) {
        for (int i = 0; i < strlen(line); ++i) {
          draw_char(2 + i, y, line[i]);
        }
        y++;
      }
      fclose(file);
    } else {
      const char *message = "Erro ao abrir o arquivo de scores.";
      for (int i = 0; i < strlen(message); ++i) {
        draw_char(2 + i, 2, message[i]);
      }
    }

    const char *back_message = "1. Voltar ao Menu";
    int back_message_length = strlen(back_message);
    int start_x = (WIDTH - back_message_length) / 2;
    int start_y = HEIGHT - 2;
    for (int i = 0; i < back_message_length; ++i) {
      draw_char(start_x + i, start_y, back_message[i]);
    }
    screenUpdate();
    usleep(400000);
    if (keyhit()) {
      int ch = readch();
      if (ch == '1') {
        break; // Voltar ao menu
      }
    }
  }
}

void menu() {
  while (1) {
    screenClear();
    screenInit(1);
    const char *menu_items[] = {"1. Jogar", "2. Ver Ranking", "3. Sair"};
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);
    int start_y = HEIGHT / 2 - num_items;
    for (int i = 0; i < num_items; ++i) {
      int start_x = (WIDTH - strlen(menu_items[i])) / 2;
      for (int j = 0; j < strlen(menu_items[i]); ++j) {
        draw_char(start_x + j, start_y + i, menu_items[i][j]);
      }
      screenUpdate();
    }
    usleep(300000);
    screenUpdate();
    if (keyhit()) {
      int ch = readch();
      if (ch == '1') {
        break; // Começar o jogo
      } else if (ch == '2') {
        view_ranking();
      } else if (ch == '3') {
        acaba();
        exit(0);
      }
    }
  }
}

int main() {
  screenInit(1);
  timerInit(0);
  keyboardInit(0);
  screenSetColor(GREEN, BLACK);

  menu();

  get_player_name();

  Player *player;
  Enemy *enemies;
  Projectile *playerProjectiles;
  Projectile *enemyProjectiles;

  initialize(&player, &enemies, &playerProjectiles, &enemyProjectiles);

  while (1) {
    desenho(*player, enemies, playerProjectiles, enemyProjectiles);
    atualiza(player, enemies, playerProjectiles, enemyProjectiles);
    usleep(60000);
  }

  // Liberar memória alocada dinamicamente
  free(player);
  free(enemies);
  free(playerProjectiles);
  free(enemyProjectiles);

  acaba();
  return 0;
}