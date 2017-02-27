#include "point.hpp"

Point::Point(int row, int col)
{
  this->row = row;
  this->col = col;
}

void Point::setDirStatus(int dir, int offset, STATUS* status)
{
  dirStatus[dir][offset] = status;
}

void Point::reset()
{
  playNo = 0;
  color = EMPTY;
}

void Point::play(STATUS color, int playNo)
{
  this->color = color;
  this->playNo = playNo;
}