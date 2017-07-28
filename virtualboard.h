#ifndef VIRTUALBOARD_H_
#define VIRTUALBOARD_H_

class VirtualBoard {
 public:
  virtual inline ~VirtualBoard() = 0;

  // virtual constructor
  virtual VirtualBoard* create() = 0;
  virtual VirtualBoard* clone() = 0;

  // get score at "index"
  virtual int getScore(int index) const = 0;

  // get the sume of every point's score
  virtual int getScoreSum() const = 0;

  // get who turn, black = 0, white = 1
  virtual bool whoTurn() const = 0;

  // get the highest score's index, return -1 if no useful point
  virtual int getHSI() const = 0;

  // get the highset score's index
  // only check if the corresponding "ignoreIndex[index]" is false
  virtual int getHSI(bool ignoreIndex[]) const = 0;

  // puts a new chess at "index"
  // returns 1 if wins after play, -1 if lose
  virtual int play(int index) = 0;

  // pass
  // return -1 if not allow pass, other for the index of the pass
  virtual int pass() = 0;

  // remove chess at "index"
  virtual void undo(int index) = 0;

 protected:
  bool isInit_ = false;
};

VirtualBoard::~VirtualBoard() {}

#endif
