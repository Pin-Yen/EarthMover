#ifndef GO_VIRTUALBOARDGO_H_
#define GO_VIRTUALBOARDGO_H_

#include <set>

#include "../virtualboard.h"

class VirtualBoardGo : public VirtualBoard {
 public:
  VirtualBoardGo();
  // copy constructor
  VirtualBoardGo(const VirtualBoardGo& source);

  ~VirtualBoardGo() override;

 private:
  static const int DIMEN = 19, LENGTH = 361;

  class Point {
   public:
    Point() : status_(EMPTY) {}

    Point(const Point &src) : status_(src.status_) {}

    StoneStatus status() const { return status_; }
    void setStatus(StoneStatus status) { status_ = status; }

   private:
    StoneStatus status_;
  };

  int length() const final { return LENGTH; }

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

  bool outOfBound(int r, int c) const {
    return r < 0 || r >= DIMEN || c < 0 || c >= DIMEN;
  }

  void removeIfDead(int row, int col, StoneStatus color);

  bool noLiberty(int row, int col, StoneStatus color,
                 std::set<int> *group) const;

  // point array
  Point point_[LENGTH];

  // Ko point, -1 means that no ko currently.
  int koPoint_;

  // the total number of plays
  int playNo_;
};

#endif  // GO_VIRTUALBOARDGO_H_
