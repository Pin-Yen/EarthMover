#include "chesstype.hpp"
#include "status.hpp"
#include "virtualboard.hpp"
#include "point.hpp"

VirtualBoard::Point::Point(int row, int col) {
  this->row = row;
  this->col = col;

  playNo = 0;
  status = EMPTY;
}

VirtualBoard::Point::Point(Point* source) {
  row = source->getRow();
  col = source->getColumn();
  playNo = source->getPlayNo();

  status = source->status;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = new ChessType(source->type[dir][i]->length, source->type[dir][i]->life);
}

void VirtualBoard::Point::reset() {
  playNo = 0;
  status = EMPTY;
}

void VirtualBoard::Point::play(STATUS status, int playNo) {
  this->status = status;
  this->playNo = playNo;
}

void VirtualBoard::Point::getDirStatusArray(int dir, STATUS* dest) {
  for (int i = 0; i < 8; ++i)
    dest[i] = *(dirStatus[dir][i]);
}