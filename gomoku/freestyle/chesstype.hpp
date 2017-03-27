#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#include "../../objectcounter.hpp"

class ChessType {
 public:
  ChessType(int length, bool life, int level) {
    length_ = length;
    life_ = life;
    level_ = level;

    ObjectCounter::registerCT();
  }

  ChessType(ChessType *source) {
    length_ = source->length_;
    life_ = source->life_;
    level_ = source->level_;

    ObjectCounter::registerCT();
  }

  ~ChessType() { ObjectCounter::unregisterCT(); }

  int length() const { return length_; }

  int level() const { return level_; }

  bool life() const { return life_; }

  bool operator==(const ChessType &type) {
    return (length_ == type.length_ && life_ == type.life_ && level_ == type.level_);
  }

  bool operator!=(const ChessType &type) {
    return (length_ != type.length_ || life_ != type.life_ || level_ != type.level_);
  }

  bool operator>(const ChessType &type) {
    return ((length_ > type.length_) || ((length_ == type.length_) && (life_ > type.life_)));
  }

  bool operator<(const ChessType &type) {
    return ((length_ < type.length_) || ((length_ == type.length_) && (life_ < type.life_)));
  }

  bool operator>=(const ChessType &type) {
    return ((length_ > type.length_) || ((length_ == type.length_) && (life_ >= type.life_)));
  }

  bool operator<=(const ChessType &type) {
    return ((length_ < type.length_) || ((length_ == type.length_) && (life_ <= type.life_)));
  }

 private:
  int length_, level_;
  bool life_;
};

#endif