#ifndef GOMOKU_VIRTUAL_BOARD_H
#define GOMOKU_VIRTUAL_BOARD_H

template <int StatusLength, class Evaluator>
class VirtualBoardGomoku : public VirtualBoard {
 public:
  static const int CHESSBOARD_DIMEN = 15,
                   OFFSET = StatusLength / 2;

  VirtualBoardGomoku();
  /* copy the source board to consturct the board */
  VirtualBoardGomoku(VirtualBoard* source);

  ~VirtualBoardGomoku();

  int getScore(int row, int col);

  /* get the sume of every point's score */
  int getScoreSum();

  /* get who turn, black = 0, white = 1 */
  bool whoTurn() { return (playNo_ & 1); }

  /* get the highest score's position, if every point is not empty, return false */
  bool getHSP(int &row, int &col);

  /* puts a new chess at (row ,col),
   * returns 1 if wins after play, -1 if lose */
  int play(int row, int col);

  virtual VirtualBoardGomoku* clone() override = 0;
 private:
  /* nested class */
  class Point;

  /* point array */
  Point* point_[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  /* the total number of plays */
  int playNo_;
};

#endif