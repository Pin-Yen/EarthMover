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
  row = source->getRow();
  col = source->getColumn();
  playNo = source->getPlayNo();

  source->getScore(score);

  status = source->status;

  for (int dir = 0; dir < 4; ++dir)
    for (int i = 0; i < 2; ++i)
      type[dir][i] = new ChessType(source->type[dir][i]->length, source->type[dir][i]->life);
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

int Point::getRow()
{
  return row;
}

int Point::getColumn()
{
  return col;
}

int Point::getPlayNo()
{
  return playNo;
}

void Point::getScore(int* destination)
{
  destination[0] = score[0];
  destination[1] = score[1];
}

