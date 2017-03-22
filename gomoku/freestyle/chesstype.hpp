#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#include "../../objectcounter.hpp"

struct ChessType {
  int length;
  bool life;

  ChessType(int length, bool life) {
    this->length = length;
    this->life = life;

    ObjectCounter::registerCT();
  }

  ~ChessType() { ObjectCounter::unregisterCT(); }
};

#endif