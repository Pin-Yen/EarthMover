#include "chesstype.hpp"
#include "status.hpp"
#include "virtualboard.hpp"
#include "point.hpp"

#include <cstddef>

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

VirtualBoard::Point::Point() {
  status = EMPTY;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = NULL;

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

VirtualBoard::Point::Point(Point* source) {
  status = source->status;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = new ChessType(source->type[dir][i]);

  #ifdef DEBUG
  ObjectCounter::registerPoint();
  #endif
}

VirtualBoard::Point::~Point() {
  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      if (type[dir][i] != NULL)
        delete type[dir][i];

  #ifdef DEBUG
  ObjectCounter::unregisterPoint();
  #endif
}

void VirtualBoard::Point::getDirStatusArray(int dir, STATUS* dest) {
  for (int i = 0; i < 8; ++i)
    dest[i] = (dirStatus[dir][i] == NULL ? BOUND : *(dirStatus[dir][i]));
}