#ifndef GOMOKU_POINT_H
#define GOMOKU_POINT_H

#include "status.hpp"
#include "chesstype.hpp"

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Point {
 public:
  /* constructor */
  Point();

  Point(Point* source);

  ~Point();
  /* play at this point */
  void play(STATUS status) { status_ = status; }

  /* set the status pointer */
  /* note: to finish initialize this point, shound call this to write all status pointer */
  void setDirStatus(int dir, int index, STATUS* status) { dirStatus_[dir][index] = status; }

  /* get the status by diraction, copy the pointer's value to dest */
  void getDirStatus(int dir, STATUS* dest);

  STATUS status() { return status_; }
  STATUS* statusRef() { return &status_; }

  int* absScore() { return absScore_; }
  int absScore(bool color) { return absScore_[color]; }

  int getScore() { return relScore_; }
  void setScore(int black, int white) { absScore_[0] = black; absScore_[1] = white; }

  void setRelScore(int score) { relScore_ = score; }

  ChessType* type[4][2];
 private:
  /* STATUS array pointer, this will point at other point's Status color */
  /* index: 0→ 1↓ 2↗ 3↘ */
  STATUS* dirStatus_[4][StatusLength];

  /* point's status, the target of the other point's STATUS array pointer*/
  STATUS status_;

  int absScore_[2];
  int relScore_;
};

#include "chesstype.hpp"
#include "status.hpp"
#include "../virtualboard.hpp"
#include "virtualboard.hpp"
#include "point.hpp"

#include <cstddef>

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::Point() {
  status_ = EMPTY;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = NULL;

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::Point(Point* source) {
  status_ = source->status_;

  absScore_[0] = source->absScore_[0]; absScore_[1] = source->absScore_[1];
  relScore_ = source->relScore_;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = new ChessType(source->type[dir][i]);

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::Point::~Point() {
  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      if (type[dir][i] != NULL)
        delete type[dir][i];

  #ifdef DEBUG
  ObjectCounter::unregisterPoint();
  #endif
}

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Point::getDirStatus(int dir, STATUS* dest) {
  for (int i = 0; i < StatusLength; ++i)
    dest[i] = (dirStatus_[dir][i] == NULL ? BOUND : *(dirStatus_[dir][i]));
}

#endif