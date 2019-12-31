enum Direction {LEFT, RIGHT, UP, DOWN};
enum State {PLAY, END};

struct Game {
  int board[4][4];
  enum State state;
  int highest_tile;
  int score;
};

struct GameLibSpec {
  struct Game (*init)();
  struct Game (*step)();
  void (*print)(struct Game);
};
extern const struct GameLibSpec GameLib;

struct Game add_new_tile(struct Game game);
