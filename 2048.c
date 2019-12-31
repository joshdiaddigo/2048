
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "game.h"
#include "work.h"

int rand_fd = -2;

void init_rand() {
  rand_fd = open("/dev/urandom", O_RDONLY);

  if (rand_fd == -1) {
    printf("Error opening [/dev/urandom]: %i\n", rand_fd);
    exit(1);
  }
}

int get_rand() {
  unsigned long buf_impl;
  unsigned long *buf = &buf_impl;

  if (rand_fd == -2) {
    init_rand();
  }

  read(rand_fd, buf, sizeof(long));
  return abs(buf_impl);
}

struct FinishGameRandomlyArgs {
  struct Game game;
  int *total_game_score;
};

void finish_game_randomly(void *void_args_p) {
  struct FinishGameRandomlyArgs *args_p = (struct FinishGameRandomlyArgs *)void_args_p;

  struct Game game = args_p->game;
  while (game.state != END) {
    enum Direction dir = (enum Direction) (rand() % 4);
    game = GameLib.step(game, dir);
  }
  *args_p->total_game_score += game.score;
}

enum Direction find_best_move(struct Game game, int look_ahead_depth) {
  int games_to_play = 100;
  int scores[] = {0, 0, 0, 0};
  int num_work_items = 4 * games_to_play;
  struct FinishGameRandomlyArgs *args[num_work_items];

  for (int i = 0; i < num_work_items; i++) {
    struct Game sub_game = GameLib.step(game, (enum Direction)(i % 4));
    for (int j = look_ahead_depth - 1; j > 0; j--) {
      sub_game = GameLib.step(sub_game, find_best_move(sub_game, j));
    }

    struct FinishGameRandomlyArgs *args_p = malloc(sizeof(struct FinishGameRandomlyArgs));
    args_p->game = sub_game;
    args_p->total_game_score = &scores[i % 4];
    args[i] = args_p;
  }

  WorkLib.do_batch(num_work_items, &finish_game_randomly, (void *) args);

  int max_score_index = 0;
  int max_score = scores[max_score_index];
  for (int i = 1; i < 4; i++) {
      if (scores[i] > max_score) {
          max_score = scores[i];
          max_score_index = i;
      }
  }

  return (enum Direction) max_score_index;
}

int main() {
  srand(get_rand());

  printf("init game\n");
  struct Game game = GameLib.init();

  printf("init workers\n");
  WorkLib.init();

  GameLib.print(game);
  while (game.state != END) {
    enum Direction dir = find_best_move(game, 1);

    if (dir == UP) {
      printf("up:\n\n");
    } else if (dir == DOWN) {
      printf("down:\n\n");
    } else if (dir == LEFT) {
      printf("left:\n\n");
    } else if (dir == RIGHT) {
      printf("right:\n\n");
    }

    game = GameLib.step(game, dir);
    GameLib.print(game);
  }

  printf("Highest Tile:\t%d\n\n", game.highest_tile);
  printf("Score:\t%d\n\n", game.score);

  return 0;
}
