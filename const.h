#ifndef CONST_H_
#define CONST_H_

// game rule ids
enum Rules {GOMOKU_FREESTYLE, GOMOKU_RENJU_BASIC};

enum GameStatus { LOSING = -1, NOTHING = 0, WINNING = 1 };

enum SearchStatus { LOSE = -1, TIE = 0, WIN = 1, UNKNOWN, LEAF };

enum StoneStatus { BLACK = 0, WHITE = 1, EMPTY = 2, BOUND = 3 };

#endif  // CONST_H_
