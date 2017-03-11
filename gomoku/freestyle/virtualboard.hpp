#include "point.hpp"
#include "evaluator.hpp"
#include "status.hpp"
#include <iostream>

class VirtualBoard
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  VirtualBoard();
  /* copy the source board to consturct the board*/
  VirtualBoard(VirtualBoard* source);

  int getScore(int row, int col, bool black) { return point[row][col]->score[black]; }

  /* puts a new chess */
  void play(int row, int col);
private:
  /* point array */
  Point* point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
  int score[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN][2];
  /* the total number of plays */
  int playNo;
};
