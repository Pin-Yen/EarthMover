#ifndef GOMOKU_VIRTUAL_BOARD_H
#define GOMOKU_VIRTUAL_BOARD_H

class VirtualBoardGomoku : public VirtualBoard<8> {
 public:
  static const int CHESSBOARD_DIMEN = 15;

  VirtualBoardGomoku();
  /* copy the source board to consturct the board */
  VirtualBoardGomoku(VirtualBoardGomoku* source);

  ~VirtualBoardGomoku() override;

  int getScore(int row, int col) override;

  /* get the sume of every point's score */
  int getScoreSum() override;

  /* get who turn, black = 0, white = 1 */
  bool whoTurn() override { return (playNo_ & 1); }

  /* get the highest score's position, if every point is not empty, return false */
  bool getHSP(int &row, int &col) override;

  /* puts a new chess at (row ,col),
   * returns 1 if wins after play, -1 if lose */
  int play(int row, int col) override;

 private:
  /* nested class */
  class Evaluator;
  class Point;

  /* point array */
  Point* point_[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  /* the total number of plays */
  int playNo_;
};

#endif