#include "chesstype.hpp"
#include "status.hpp"
#include "virtualboard.hpp"
#include "point.hpp"
#include "../../objectcounter.hpp"

#include <cstddef>
VirtualBoard::Point::Point() {
  status = EMPTY;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = NULL;
}

VirtualBoard::Point::Point(Point* source) {
  status = source->status;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = new ChessType(source->type[dir][i]->length, source->type[dir][i]->life);

  ObjectCounter::registerPoint();
}

VirtualBoard::Point::~Point(){
  ObjectCounter::unregisterPoint();

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      if (type[dir][i] != NULL)
        delete type[dir][i];
}

void VirtualBoard::Point::getDirStatusArray(int dir, STATUS* dest) {
  for (int i = 0; i < 8; ++i)
    dest[i] = (dirStatus[dir][i] == NULL ? BOUND : *(dirStatus[dir][i]));
}