#ifndef GOMOKU_POINT_H
#define GOMOKU_POINT_H

#include "chesstype.hpp"
#include "status.hpp"
#include "virtualboardgomoku.hpp"

#include <cstddef>

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Point {
 public:
  Point();
  Point(const Point& source);

  ~Point() {
    #ifdef DEBUG
    ObjectCounter::unregisterPoint();
    #endif
  }

  // set the status pointer
  // note: to finish initialize this point, shound call this to write all status pointer
  void setDirStatus(int dir, int index, const STATUS* status) { dirStatus_[dir][index] = status; }

  // get the status by diraction, copy the pointer's value to dest
  void getDirStatus(int dir, STATUS* dest) const {
    for (int i = 0; i < StatusLength; ++i)
      dest[i] = (dirStatus_[dir][i] == NULL ? BOUND : *(dirStatus_[dir][i]));
  }

  void setStatus(STATUS status) { status_ = status; }
  STATUS status() const { return status_; }
  const STATUS* statusRef() { return &status_; }

  int* absScore() { return absScore_; }
  int absScore(bool color) const { return absScore_[color]; }

  int score() const { return score_; }
  void setScore(int score) { score_ = score; }

  void setAbsScore(int black, int white) { absScore_[0] = black; absScore_[1] = white; }

  ChessType& type(int index) { return type_[index]; }
  ChessType* type() { return type_; }
 private:
  // chess type array
  // index: 0→ 1↓ 2↗ 3↘
  ChessType type_[4];

  // STATUS array pointer, this will point at other point's Status color
  // index: 0→ 1↓ 2↗ 3↘
  const STATUS* dirStatus_[4][StatusLength];

  // point's status, the target of the other point's STATUS array pointer
  STATUS status_;

  int absScore_[2];
  int score_;
};

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::Point(): status_(EMPTY) {
  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::Point(const Point& source):
  status_(source.status_),
  score_(source.score_),
  absScore_{ source.absScore_[0], source.absScore_[1] },
  type_{ source.type_[0], source.type_[1], source.type_[2], source.type_[3] } {

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

#endif