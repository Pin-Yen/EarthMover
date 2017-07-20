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

  /* get the highest score's index, return -1 if no useful point */
  virtual int getHSI() const = 0;

  /* puts a new chess at "index",
   * returns 1 if wins after play, -1 if lose */
  virtual int play(int index) = 0;

  /* pass */
  virtual void pass() = 0;

  /* remove chess at "index" */
  virtual void undo(int index) = 0;
 protected:
  bool isInit_ = false;
};

VirtualBoard::~VirtualBoard() {}

#endif