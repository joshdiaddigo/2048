#include <stdio.h>
#include <stdlib.h>
#include "game.h"

struct Game init_game() {
  struct Game game = {
    .board = {
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}
    },
    .state = PLAY,
    .highest_tile = 0
  };

  game = add_new_tile(game);
  game = add_new_tile(game);

  return game;
}

struct Game add_new_tile(struct Game game) {
  int x_pos;
  int y_pos;
  int val;

  do {
    x_pos = rand() % 4;
    y_pos = rand() % 4;
  } while (game.board[x_pos][y_pos] != 0);

  val = (rand() % 10) >= 9 ? 4 : 2;
  game.board[x_pos][y_pos] = val;
  return game;
}

struct Game step_game(struct Game game, enum Direction dir) {
  int changed = 0;
  if (dir == LEFT) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 3; j++) {
        if (game.board[i][j] == 0) { // slide the next value
          for (int k = j + 1; k < 4; k++) {
            if (game.board[i][k] != 0) {
              game.board[i][j] = game.board[i][k];
              game.board[i][k] = 0;
              changed = 1;
              break;
            }
          }
        }
        // try merging
        if (game.board[i][j] == 0) break; // don't merge 0s to avoid "changes"
        for (int k = j + 1; k < 4; k++) {
          if (game.board[i][j] == game.board[i][k]) {
            game.board[i][j] = game.board[i][j] << 1;
            game.score += game.board[i][j];
            if (game.board[i][j] > game.highest_tile) {
              game.highest_tile = game.board[i][j];
            }
            game.board[i][k] = 0;
            changed = 1;
            break;
          } else if (game.board[i][k] != 0) {
            break;
          }
        }
      }
    }
  } else if (dir == RIGHT) {
    for (int i = 0; i < 4; i++) {
      for (int j = 3; j >= 0; j--) {
        if (game.board[i][j] == 0) { // slide the next value
          for (int k = j - 1; k >= 0; k--) {
            if (game.board[i][k] != 0) {
              game.board[i][j] = game.board[i][k];
              game.board[i][k] = 0;
              changed = 1;
              break;
            }
          }
        }
        // try merging
        if (game.board[i][j] == 0) break; // don't merge 0s to avoid "changes"
        for (int k = j - 1; k >= 0; k--) {
          if (game.board[i][j] == game.board[i][k]) {
            game.board[i][j] = game.board[i][j] << 1;
            game.score += game.board[i][j];
            if (game.board[i][j] > game.highest_tile) {
              game.highest_tile = game.board[i][j];
            }
            game.board[i][k] = 0;
            changed = 1;
            break;
          } else if (game.board[i][k] != 0) {
            break;
          }
        }
      }
    }
  } else if (dir == UP) {
    for (int j = 0; j < 4; j++) {
      for (int i = 0; i < 3; i++) {
        if (game.board[i][j] == 0) { // slide the next value
          for (int k = i + 1; k < 4; k++) {
            if (game.board[k][j] != 0) {
              game.board[i][j] = game.board[k][j];
              game.board[k][j] = 0;
              changed = 1;
              break;
            }
          }
        }
        // try merging
        if (game.board[i][j] == 0) break; // don't merge 0s to avoid "changes"
        for (int k = i + 1; k < 4; k++) {
          if (game.board[i][j] == game.board[k][j]) {
            game.board[i][j] = game.board[i][j] << 1;
            game.score += game.board[i][j];
            if (game.board[i][j] > game.highest_tile) {
              game.highest_tile = game.board[i][j];
            }
            game.board[k][j] = 0;
            changed = 1;
            break;
          } else if (game.board[i][k] != 0) {
            break;
          }
        }
      }
    }
  } else if (dir == DOWN) {
    for (int j = 0; j < 4; j++) {
      for (int i = 3; i > 0; i--) {
        if (game.board[i][j] == 0) { // slide the next value
          for (int k = i - 1; k >= 0; k--) {
            if (game.board[k][j] != 0) {
              game.board[i][j] = game.board[k][j];
              game.board[k][j] = 0;
              changed = 1;
              break;
            }
          }
        }
        // try merging
        if (game.board[i][j] == 0) break; // don't merge 0s to avoid "changes"
        for (int k = i - 1; k >= 0; k--) {
          if (game.board[i][j] == game.board[k][j]) {
            game.board[i][j] = game.board[i][j] << 1;
            game.score += game.board[i][j];
            if (game.board[i][j] > game.highest_tile) {
              game.highest_tile = game.board[i][j];
            }
            game.board[k][j] = 0;
            changed = 1;
            break;
          } else if (game.board[i][k] != 0) {
            break;
          }
        }
      }
    }
  }

  if (changed) {
    game = add_new_tile(game);
  } else {
    game.state = END;
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (game.board[i][j] == 0) {
          game.state = PLAY;
        }

        if (j == 0) {
          if (game.board[i][j] == game.board[i][j + 1]) {
            game.state = PLAY;
          }
        } else if (j == 3) {
          if (game.board[i][j] == game.board[i][j - 1]) {
            game.state = PLAY;
          }
        } else {
          if (game.board[i][j] == game.board[i][j + 1]
                || game.board[i][j] == game.board[i][j + 1]) {
            game.state = PLAY;
          }
        }

        if (i == 0) {
          if (game.board[i][j] == game.board[i + 1][j]) {
            game.state = PLAY;
          }
        } else if (i == 3) {
          if (game.board[i][j] == game.board[i - 1][j]) {
            game.state = PLAY;
          }
        } else {
          if (game.board[i][j] == game.board[i + 1][j]
                || game.board[i][j] == game.board[i - 1][j]) {
            game.state = PLAY;
          }
        }
      }
    }
  }

  return game;
}

void print_game(struct Game game) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game.board[i][j] == 0) {
        printf("\t");
      } else {
        printf("%d\t", game.board[i][j]);
      }
    }
    printf("\n");
  }
  printf("____________________________\n\n");
}

const struct GameLibSpec GameLib = {
    .init = init_game,
    .step = step_game,
    .print = print_game
};
