// /**
//  * main.h
//  * Criado em 23 de agosto de 2023
//  * Autor: Tiago Barros
//  * Baseado no curso "De C para C++ - 2002"
//  */

// #include "keyboard.h"
// #include "screen.h"
// #include "timer.h"
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h> // Para a função usleep

// typedef struct {
//   int x, y;
// } Position;

// typedef struct {
//   Position pos;
// } Player;

// typedef struct {
//   Position pos;
//   int active;
// } Projectile;

// typedef struct {
//   Position pos;
//   int alive;
// } Enemy;

// #define ENEMIES_PER_ROW 10
// #define NUM_ROWS 5
// #define NUM_ENEMIES (ENEMIES_PER_ROW * NUM_ROWS)
// #define MAX_PROJECTILES 1
// #define WIDTH 81
// #define HEIGHT 24

// // Função para inicializar a posição do jogador, o número de inimigos e o estado
// // dos projéteis como inativos
// void initialize(Player *player, Enemy enemies[], Projectile projectiles[]) {
//   player->pos.x = WIDTH / 2;
//   player->pos.y = HEIGHT - 3;

//   for (int i = 0; i < NUM_ENEMIES; ++i) {
//     enemies[i].pos.x = (i % ENEMIES_PER_ROW + 1) * (WIDTH / (ENEMIES_PER_ROW + 1));
//     enemies[i].pos.y = 3 + (i / ENEMIES_PER_ROW);
//     enemies[i].alive = 1;
//   }
//   for (int i = 0; i < MAX_PROJECTILES; ++i) {
//     projectiles[i].active = 0;
//   }
// }

// void desenho(Player player, Enemy enemies[], Projectile projectiles[]) {
//   screenClear();

//   // Desenha limites da tela
//   for (int i = 0; i < WIDTH; ++i) {
//     draw_char(i, HEIGHT - 1, '-'); // Linha superior
//     draw_char(i, 0, '-');          // Linha inferior
//   }
//   for (int i = 0; i < HEIGHT; ++i) {
//     draw_char(0, i, '|');         // Limite esquerdo
//     draw_char(WIDTH - 1, i, '|'); // Limite direito
//   }

//   // Desenha o jogador
//   draw_char(player.pos.x, player.pos.y, 'W');

//   // Desenha inimigos
//   for (int i = 0; i < NUM_ENEMIES; ++i) {
//     if (enemies[i].alive) {
//       draw_char(enemies[i].pos.x, enemies[i].pos.y, 'M');
//     }
//   }

//   // Desenha projéteis
//   for (int i = 0; i < MAX_PROJECTILES; ++i) {
//     if (projectiles[i].active) {
//       draw_char(projectiles[i].pos.x, projectiles[i].pos.y, '!');
//     }
//   }
//   screenUpdate();
// }

// void atualiza(Player *player, Enemy enemies[], Projectile projectiles[]) {
//   // Atualiza os projéteis
//   for (int i = 0; i < MAX_PROJECTILES; ++i) {
//     if (projectiles[i].active) {
//       projectiles[i].pos.y--;
//       if (projectiles[i].pos.y < 1) {
//         projectiles[i].active = 0;
//       }
//       // Verifica colisão com inimigos
//       for (int j = 0; j < NUM_ENEMIES; ++j) {
//         if (enemies[j].alive && projectiles[i].pos.x == enemies[j].pos.x &&
//             projectiles[i].pos.y == enemies[j].pos.y) {
//           enemies[j].alive = 0;
//           projectiles[i].active = 0;
//         }
//       }
//     }
//   }

//   // Movimentos do jogador e disparo do projétil (simplificado)
//   int ch = getchar();
//   if (ch == 'a' && player->pos.x > 1)
//     player->pos.x--;
//   if (ch == 'd' && player->pos.x < WIDTH - 2)
//     player->pos.x++;
//   if (ch == ' ') {
//     for (int i = 0; i < MAX_PROJECTILES; ++i) {
//       if (!projectiles[i].active) {
//         projectiles[i].pos.x = player->pos.x;
//         projectiles[i].pos.y = player->pos.y - 1;
//         projectiles[i].active = 1;
//         break;
//       }
//     }
//   }
// }

// void acaba() {
//   keyboardDestroy();
//   timerDestroy();
//   screenDestroy();
// }

// int main() {
//   screenInit(1);
//   timerInit(0);
//   keyboardInit(0);

//   Player player;
//   Enemy enemies[NUM_ENEMIES];
//   Projectile projectiles[MAX_PROJECTILES];

//   initialize(&player, enemies, projectiles);

//   while (1) {
//     desenho(player, enemies, projectiles);
//     atualiza(&player, enemies, projectiles);
//     usleep(11100); // Atraso para controle da velocidade do jogo
//   }

//   return 0;
// }