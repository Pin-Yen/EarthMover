#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#include "../../objectcounter.hpp"

class ChessType {
 public:
  ChessType(int length, bool life) {
    length_ = length;
    life_ = life;

    ObjectCounter::registerCT();
  }

  ChessType(ChessType *source) {
    length_ = source->length();
    life_ = source->life();

    ObjectCounter::registerCT();
  }

  ~ChessType() { ObjectCounter::unregisterCT(); }

  int length() const { return length_; }

  bool life() const { return life_; }

  bool operator!=(const ChessType &type) {
    return (length_ != type.length_ || life_ != type.life_);
  }

  bool operator<(const ChessType &type) {
    return ((length_ < type.length_) || (length_ == type.length_) && (life_ < type.life_));
  }

 private:
  int length_;
  bool life_;
};

#endif