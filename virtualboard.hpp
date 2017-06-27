#ifndef VIRTUAL_BOARD_H
#define VIRTUAL_BOARD_H

class VirtualBoard {
 public:
  virtual inline ~VirtualBoard() = 0;

  virtual VirtualBoard* create() = 0;

  virtual VirtualBoard* clone() = 0;

  virtual int getScore(int index) const = 0;

  /* get the sume of every point's score */
  virtual int getScoreSum() const = 0;

  /* get who turn, black = 0, white = 1 */
  virtual bool whoTurn() const = 0;

  /* get the highest score's position, if every point is not empty, return false */
  virtual bool getHSP(int* index) const = 0;

  /* puts a new chess at (row ,col),
   * returns 1 if wins after play, -1 if lose */
  virtual int play(int index) = 0;

  /* remove chess at (row, col) */
  virtual void undo(int index) = 0;
 protected:
  bool isInit = false;
};

VirtualBoard::~VirtualBoard() {}

#endif