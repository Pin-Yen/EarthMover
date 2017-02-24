#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

struct ChessType
{
  int length, life;

  ChessType(int length, bool life)
  {
    this->length = length;
    this->life = life;
  }
};

#endif