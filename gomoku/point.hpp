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
  Point(Point* source);

  ~Point();

  // set the status pointer *
  // note: to finish initialize this point, shound call this to write all status pointer
  void setDirStatus(int dir, int index, STATUS* status) { dirStatus_[dir][index] = status; }

  // get the status by diraction, copy the pointer's value to dest
  void getDirStatus(int dir, STATUS* dest) const {
    for (int i = 0; i < StatusLength; ++i)
      dest[i] = (dirStatus_[dir][i] == NULL ? BOUND : *(dirStatus_[dir][i]));
  }

  void setStatus(STATUS status) { status_ = status; }
  STATUS status() const { return status_; }
  STATUS* statusRef() { return &status_; }

  int* absScore() { return absScore_; }
  int absScore(bool color) const { return absScore_[color]; }

  int score() const { return score_; }
  void setScore(int score) { score_ = score; }

  void setAbsScore(int black, int white) { absScore_[0] = black; absScore_[1] = white; }

  ChessType type[4];
 private:
  // STATUS array pointer, this will point at other point's Status color
  // index: 0→ 1↓ 2↗ 3↘
  STATUS* dirStatus_[4][StatusLength];

  // point's status, the target of the other point's STATUS array pointer
  STATUS status_;

  int absScore_[2];
  int score_;
};

#include "../virtualboard.hpp"
#include "virtualboardgomoku.hpp"

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::Point() {
  status_ = EMPTY;

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::Point(Point* source) {
  status_ = source->status_;

  absScore_[0] = source->absScore_[0]; absScore_[1] = source->absScore_[1];
  score_ = source->score_;

  for (int dir = 0; dir < 4; ++dir)
    type[dir] = ChessType(source->type[dir]);

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::~Point() {

  #ifdef DEBUG
  ObjectCounter::unregisterPoint();
  #endif
}

#endif