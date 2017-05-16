#include "chesstype.hpp"
#include "status.hpp"
#include "../virtualboard.hpp"
#include "virtualboard.hpp"
#include "point.hpp"

#include <cstddef>

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

template <int StatusLength, class Eva>
VirtualBoardGomoku<StatusLength, Eva>::Point::Point() {
  status_ = EMPTY;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = NULL;

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

template <int StatusLength, class Eva>
VirtualBoardGomoku<StatusLength, Eva>::Point::Point(Point* source) {
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

template <int StatusLength, class Eva>
VirtualBoardGomoku<StatusLength, Eva>::Point::~Point() {
  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      if (type[dir][i] != NULL)
        delete type[dir][i];

  #ifdef DEBUG
  ObjectCounter::unregisterPoint();
  #endif
}

template <int StatusLength, class Eva>
void VirtualBoardGomoku<StatusLength, Eva>::Point::getDirStatus(int dir, STATUS* dest) {
  for (int i = 0; i < StatusLength; ++i)
    dest[i] = (dirStatus_[dir][i] == NULL ? BOUND : *(dirStatus_[dir][i]));
}