#ifndef VIRTUAL_BOARD_H
#define VIRTUAL_BOARD_H

class VirtualBoard {
 public:
  virtual int getScore(int row, int col) = 0;

  /* get the sume of every point's score */
  virtual int getScoreSum() = 0;

  /* get who turn, black = 0, white = 1 */
  virtual bool whoTurn() = 0;

  /* get the highest score's position, if every point is not empty, return false */
  virtual bool getHSP(int &row, int &col) = 0;

  /* puts a new chess at (row ,col),
   * returns 1 if wins after play, -1 if lose */
  virtual int play(int row, int col) = 0;

  class Evaluator;
};

#endif