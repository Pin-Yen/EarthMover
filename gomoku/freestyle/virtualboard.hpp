#include "point.hpp"
#include "evaluator.hpp"
#include "status.hpp"
#include <random>
#include <iostream>

class VirtualBoard
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  VirtualBoard();
  /* copy the source board to consturct the board */
  VirtualBoard(VirtualBoard* source);

  int getScore(int row, int col, bool color) { return score[row][col][color]; }

  /* get the highest score's position */
  void getHSP(int &row, int &col, bool color);

  /* puts a new chess if win after play then return true */
  bool play(int row, int col);
private:
  /* point array */
  Point* point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
  int score[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN][2];
  /* the total number of plays */
  int playNo;
};
