#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estruturas para as definições dos elementos
typedef struct {
  int x, y;
} Posicao;

typedef struct {
  Posicao pos;
} Jogador;

typedef struct {
  Posicao pos;
  int ativo;
} Projetil;

typedef struct {
  Posicao pos;
  int vivo;
} Inimigo;

// Declarações de função (prototypes)
void inicializa(Jogador **jogador, Inimigo **inimigos, Projetil **projeteisJogador, Projetil **projeteisInimigo);
void desenho(Jogador jogador, Inimigo inimigos[], Projetil projeteisJogador[], Projetil projeteisInimigo[]);
void atualiza(Jogador *jogador, Inimigo inimigos[], Projetil projeteisJogador[], Projetil projeteisInimigo[]);
void finaliza();
void pega_nome_jogador();
void menu();
void desenha_caractere(int x, int y, char caractere);
void desenha_game_over();
void desenha_mensagem_vitoria();
void desenha_pontuacao();
void salva_pontuacao();
void ver_ranking();

// Variáveis globais
#define INIMIGOS_POR_LINHA 6
#define NUM_LINHAS 4
#define NUM_INIMIGOS (INIMIGOS_POR_LINHA * NUM_LINHAS)
#define MAX_PROJETEIS 2
#define LARGURA 81
#define ALTURA 24

// Inicialização de elementos
int pontuacao = 0;
char nomeJogador[20];
int timerMovimentoInimigo = 0;
int contadorMovimentoProjetilInimigo = 0;
int direcaoInimigo = 1;
time_t ultimaMovimentacaoInimigo = 0;

int main(){
  screenInit(1);
  timerInit(0);
  keyboardInit(0);

  menu();
  pega_nome_jogador();

  Jogador *jogador;
  Inimigo *inimigos;
  Projetil *projeteisJogador;
  Projetil *projeteisInimigo;

  inicializa(&jogador, &inimigos, &projeteisJogador, &projeteisInimigo);

  while (1){
    desenho(*jogador, inimigos, projeteisJogador, projeteisInimigo);
    atualiza(jogador, inimigos, projeteisJogador, projeteisInimigo);
    usleep(60000);
  }

  // Liberar memória alocada dinamicamente
  free(jogador);
  free(inimigos);
  free(projeteisJogador);
  free(projeteisInimigo);

  finaliza();
  return 0;
}

// Funções do Código:

// Função de inicialização dos elementos e do jogo em si
void inicializa(Jogador **jogador, Inimigo **inimigos, Projetil **projeteisJogador, Projetil **projeteisInimigo){

  *jogador = (Jogador *)malloc(sizeof(Jogador));
  (*jogador)->pos.x = LARGURA / 2;
  (*jogador)->pos.y = ALTURA - 3;

  *inimigos = (Inimigo *)malloc(NUM_INIMIGOS * sizeof(Inimigo));
  for (int i = 0; i < NUM_INIMIGOS; ++i){
    (*inimigos)[i].pos.x = (i % INIMIGOS_POR_LINHA + 1) * (LARGURA / (INIMIGOS_POR_LINHA + 1));
    (*inimigos)[i].pos.y = 3 + (i / INIMIGOS_POR_LINHA);
    (*inimigos)[i].vivo = 1;
  }

  *projeteisJogador = (Projetil *)malloc(MAX_PROJETEIS * sizeof(Projetil));
  *projeteisInimigo = (Projetil *)malloc(MAX_PROJETEIS * sizeof(Projetil));

  for (int i = 0; i < MAX_PROJETEIS; ++i){
    (*projeteisJogador)[i].ativo = 0;
    (*projeteisInimigo)[i].ativo = 0;
  }
  ultimaMovimentacaoInimigo = time(NULL);
}

void desenha_caractere(int x, int y, char caractere){

  screenGotoxy(x, y); // Move o cursor para a posição (x, y)
  putchar(caractere); // Desenha o caractere
}

// Função de GAME OVER caso jogador seja derrotado
void desenha_game_over(){

  screenInit(1);
  const char *mensagem = "GAME-OVER";
  int tamanho_mensagem = strlen(mensagem);
  int inicio_x = (LARGURA - tamanho_mensagem) / 2;
  int inicio_y = ALTURA / 2;
  screenSetColor(RED, BLACK);
  for (int i = 0; i < tamanho_mensagem; ++i){
    desenha_caractere(inicio_x + i, inicio_y, mensagem[i]);
  }
}

// Função para mensagem de Vitória após jogador eliminar todos inimigos
void desenha_mensagem_vitoria() {

  screenInit(1);
  const char *mensagem = "VOCE VENCEU!";
  int tamanho_mensagem = strlen(mensagem);
  int inicio_x = (LARGURA - tamanho_mensagem) / 2;
  int inicio_y = ALTURA / 2;
  screenSetColor(GREEN, BLACK);
  for (int i = 0; i < tamanho_mensagem; ++i){
    desenha_caractere(inicio_x + i, inicio_y, mensagem[i]);
  }
}

// Função para desenhar a pontuação no topo da tela
void desenha_pontuacao(){

  char str_pontuacao[40];
  sprintf(str_pontuacao, "Score: %d", pontuacao);
  int tamanho_mensagem = strlen(str_pontuacao);
  int inicio_x = (LARGURA - tamanho_mensagem) / 2;
  screenSetColor(YELLOW, BLACK);
  for (int i = 0; i < tamanho_mensagem; ++i){
    desenha_caractere(inicio_x + i, 0, str_pontuacao[i]);
  }
}

// Função para definição do formato dos elementos
void desenho(Jogador jogador, Inimigo inimigos[], Projetil projeteisJogador[], Projetil projeteisInimigo[]) {

  screenClear();
  screenInit(1);
  screenSetColor(GREEN, BLACK);
  desenha_caractere(jogador.pos.x, jogador.pos.y, 'W');

  for (int i = 0; i < NUM_INIMIGOS; ++i){
    if (inimigos[i].vivo){
      desenha_caractere(inimigos[i].pos.x, inimigos[i].pos.y, 'M');
    }
  }

  for (int i = 0; i < MAX_PROJETEIS; ++i){
    if (projeteisJogador[i].ativo){
      desenha_caractere(projeteisJogador[i].pos.x, projeteisJogador[i].pos.y, '!');
    }
    if (projeteisInimigo[i].ativo){
      desenha_caractere(projeteisInimigo[i].pos.x, projeteisInimigo[i].pos.y, '!');
    }
  }

  desenha_pontuacao();
  screenUpdate();
}

// Função para salvar a pontuação dos jogadores no arquivo .txt
void salva_pontuacao(){

  FILE *arquivo = fopen("scores.txt", "a");
  if (arquivo){
    fprintf(arquivo, "Player: %s, Score: %d\n", nomeJogador, pontuacao);
    fclose(arquivo);
  }else{
    printf("Erro ao abrir o arquivo para salvar o score.\n");
  }
}

// Função para atualização de informações na tela
void atualiza(Jogador *jogador, Inimigo inimigos[], Projetil projeteisJogador[], Projetil projeteisInimigo[]){

  int todosInimigosMortos =
      1; // Variável para verificar se todos os inimigos estão mortos

  for (int i = 0; i < MAX_PROJETEIS; ++i){
    if (projeteisJogador[i].ativo){
      projeteisJogador[i].pos.y--;
      if (projeteisJogador[i].pos.y < 1){
        projeteisJogador[i].ativo = 0;
      }

      for (int j = 0; j < NUM_INIMIGOS; ++j){
        if (inimigos[j].vivo && projeteisJogador[i].pos.x == inimigos[j].pos.x && projeteisJogador[i].pos.y == inimigos[j].pos.y){
          inimigos[j].vivo = 0;
          projeteisJogador[i].ativo = 0;
          pontuacao = pontuacao + 10;
        }
      }
    }

    if (projeteisInimigo[i].ativo){
      if (contadorMovimentoProjetilInimigo % 3 == 0){
        projeteisInimigo[i].pos.y++;
        if (projeteisInimigo[i].pos.y >= ALTURA - 1){
          projeteisInimigo[i].ativo = 0;
        }

        if (projeteisInimigo[i].pos.x == jogador->pos.x && projeteisInimigo[i].pos.y == jogador->pos.y){
          screenClear();
          desenha_game_over();
          screenUpdate();
          usleep(3000000);
          salva_pontuacao();
          exit(0);
        }
      }
    }
  }

  contadorMovimentoProjetilInimigo++;

  for (int i = 0; i < NUM_INIMIGOS; ++i){
    if (inimigos[i].vivo){
      todosInimigosMortos = 0; // Se encontrar um inimigo vivo, define como 0
      if (rand() % 100 < 5){
        for (int j = 0; j < 3; ++j){
          for (int k = 0; k < MAX_PROJETEIS; ++k){
            if (!projeteisInimigo[k].ativo){
              projeteisInimigo[k].pos.x = inimigos[i].pos.x;
              projeteisInimigo[k].pos.y = inimigos[i].pos.y + 1;
              projeteisInimigo[k].ativo = 1;
              break;
            }
          }
        }
      }
    }
  }

  if (todosInimigosMortos){
    screenClear();
    desenha_mensagem_vitoria();
    screenUpdate();
    usleep(3000000);
    salva_pontuacao();
    exit(0);
  }

  if (keyhit()){
    int ch = readch();
    if (ch == 'a' && jogador->pos.x > 2){
      jogador->pos.x--;
    }
    if (ch == 'd' && jogador->pos.x < LARGURA - 3){
      jogador->pos.x++;
    }
    if (ch == ' '){
      int podeAtirar = 1;
      for (int i = 0; i < MAX_PROJETEIS; ++i){
        if (projeteisJogador[i].ativo){
          podeAtirar = 0;
          break;
        }
      }
      if (podeAtirar){
        for (int i = 0; i < MAX_PROJETEIS; ++i){
          if (!projeteisJogador[i].ativo) {
            projeteisJogador[i].pos.x = jogador->pos.x;
            projeteisJogador[i].pos.y = jogador->pos.y - 1;
            projeteisJogador[i].ativo = 1;
            break;
          }
        }
      }
    }
  }
}

// Função para finalizar jogo
void finaliza(){

  keyboardDestroy();
  timerDestroy();
  screenDestroy();
}

// Função para salvar nickname do jogador
void pega_nome_jogador(){

  const char *mensagem = "Digite seu nome: ";
  int tamanho_mensagem = strlen(mensagem);
  int inicio_x = (LARGURA - tamanho_mensagem) / 2;
  int inicio_y = ALTURA / 2;

  for (int i = 0; i < tamanho_mensagem; ++i){
    desenha_caractere(inicio_x + i, inicio_y, mensagem[i]);
  }

  screenUpdate();

  int indice = 0;
  while (1){
    if (keyhit()){
      int ch = readch();
      if (ch == '\n' || ch == '\r'){
        nomeJogador[indice] = '\0';
        break;
      }else if (ch == 127 && indice > 0){ // 127 em ASCII é caractere backspace
        indice--;
        desenha_caractere(inicio_x + tamanho_mensagem + indice, inicio_y, ' ');
        screenUpdate();
      }else if (indice < sizeof(nomeJogador) - 1){
        nomeJogador[indice++] = ch;
        desenha_caractere(inicio_x + tamanho_mensagem + indice - 1, inicio_y, ch);
        screenUpdate();
      }
    }
  }
}

// Função para verificar o score de jogadores na tela
void ver_ranking(){

  while (1){
    screenClear();
    screenInit(1);
    screenSetColor(GREEN, BLACK);
    FILE *arquivo = fopen("scores.txt", "r");
    if (arquivo){
      char linha[100];
      int y = 2;
      while (fgets(linha, sizeof(linha), arquivo)){
        for (int i = 0; i < strlen(linha); ++i){
          desenha_caractere(2 + i, y, linha[i]);
        }
        y++;
      }
      fclose(arquivo);
    }else{
      const char *mensagem = "Erro ao abrir o arquivo de scores.";
      for (int i = 0; i < strlen(mensagem); ++i) {
        desenha_caractere(2 + i, 2, mensagem[i]);
      }
    }

    const char *mensagem_voltar = "1. Voltar ao Menu";
    int tamanho_mensagem_voltar = strlen(mensagem_voltar);
    int inicio_x = (LARGURA - tamanho_mensagem_voltar) / 2;
    int inicio_y = ALTURA - 2;
    for (int i = 0; i < tamanho_mensagem_voltar; ++i){
      desenha_caractere(inicio_x + i, inicio_y, mensagem_voltar[i]);
    }
    screenUpdate();
    usleep(400000);
    if (keyhit()) {
      int ch = readch();
      if (ch == '1'){
        break; // Voltar ao menu
      }
    }
  }
}

// Função para mostrar menu na tela
void menu(){

  while (1){
    screenClear();
    screenInit(1);
    screenSetColor(GREEN, BLACK);
    const char *itens_menu[] = {"1. Jogar", "2. Ver Pontuacoes", "3. Sair"};
    int num_itens = sizeof(itens_menu) / sizeof(itens_menu[0]);
    int inicio_y = ALTURA / 2 - num_itens;
    for (int i = 0; i < num_itens; ++i) {
      int inicio_x = (LARGURA - strlen(itens_menu[i])) / 2;
      for (int j = 0; j < strlen(itens_menu[i]); ++j){
        desenha_caractere(inicio_x + j, inicio_y + i, itens_menu[i][j]);
      }
      screenUpdate();
    }
    usleep(300000);
    screenUpdate();
    if (keyhit()){
      int ch = readch();
      if (ch == '1') {
        break; // Começar o jogo
      }else if (ch == '2'){
        ver_ranking();
      }else if (ch == '3'){
        finaliza();
        exit(0);
      }
    }
  }
}
