#include "point.hpp"
#include "evaluator.hpp"

#ifndef STATUS_ENUM
#define STATUS_ENUM
enum STATUS{BLACK = 0, WHITE = 1, EMPTY = 2, BOUND = 3, ANALYZE_POINT = 4, NO_MATTER = 5};
#endif

class VirtualBoard
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  VirtualBoard();

  /* puts a new chess, if the ponit is not empty then return false */
  bool play(STATUS color, int row, int col);

  /* clears the whole game */
  void wipe();

  /* get is black turn or not */
  bool isBlackTurn();

private:
  /* point array */
  Point* point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  bool blackTurn;

  /* the total number of plays */
  int playNo;

  bool checkForbidden;
};
