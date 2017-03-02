#include "point.hpp"
#include "evaluator.hpp"
#include "status.hpp"

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
