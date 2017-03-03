#include "point.hpp"

Point::Point(int row, int col)
{
  this->row = row;
  this->col = col;

  playNo = 0;
  status = EMPTY;
}

Point::Point(Point* source)
{
  //TODO: copy point
}

void Point::setDirStatus(int dir, int offset, STATUS* status)
{
  dirStatus[dir][offset] = status;
}

void Point::reset()
{
  playNo = 0;
  status = EMPTY;
}

void Point::play(STATUS status, int playNo)
{
  this->status = status;
  this->playNo = playNo;
}