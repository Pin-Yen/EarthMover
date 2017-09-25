#ifndef GO_VIRTUALBOARDGO_H_
#define GO_VIRTUALBOARDGO_H_

#include <random>
#include <iostream>

#include "../virtualboard.h"

template <int StatusLength>
class VirtualBoardGo : public VirtualBoard {
 public:
  VirtualBoardGo() {}
  // copy constructor
  VirtualBoardGo(const VirtualBoardGomoku& source);

  ~VirtualBoardGo() override;

  int length() final { return LENGTH; }

 private:
  static const int DIMEN = 19, LENGTH = 361;

  class Point;

  void init();

  GameStatus play(int index) final;

  void undo(int index) final;

  // get score at "index"
  inline int getScore(int index) const final;

  // get the sume of every point's score
  int getScoreSum() const final;

  // get the highest score's index, return -1 if no useful point
  int getHSI() const final;

  // get the highset score's index
  // only check if the corresponding "ignoreIndex[index]" is false
  int getHSI(bool ignoreIndex[]) const final;

  // get who turn, black = 0, white = 1
  bool whoTurn() const final { return (playNo_ & 1); }

  // pass
  // return the index of pass
  int pass() { ++playNo_; return LENGTH; }

  // point array
  Point point_[LENGTH];

  // the total number of plays
  int playNo_;
};

#endif  // GO_VIRTUALBOARDGO_H_
