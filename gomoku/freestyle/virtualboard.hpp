#ifndef VIRTUAL_BOARD_H
#define VIRTUAL_BOARD_H

class VirtualBoard {
 public:
  static const int CHESSBOARD_DIMEN = 15;

  VirtualBoard();
  /* copy the source board to consturct the board */
  VirtualBoard(VirtualBoard* source);

  ~VirtualBoard();

  int getScore(int row, int col);// { return score[row][col][playNo & 1]; }

  /* get the sume of every point's score */
  int getScoreSum();

  /* get who turn, black = 0, white = 1 */
  bool whoTurn() { return (playNo & 1); }

  /* get the highest score's position, if every point is not empty, return false */
  bool getHSP(int &row, int &col);

  /* puts a new chess if win after play then return true */
  bool play(int row, int col);
 private:
  /* nested class */
  class Point;
  class Evaluator;

  /* point array */
  Point* point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
  //int score[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN][2];
  //int basicScore[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN][2];

  /* the total number of plays */
  int playNo;
};
#endif