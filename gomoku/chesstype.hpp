#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

class ChessType {
 public:
  ChessType(int length, bool life, int level) {
    length_ = length;
    life_ = life;
    level_ = level;

    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  ChessType(ChessType *source) {
    length_ = source->length_;
    life_ = source->life_;
    level_ = source->level_;

    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  #ifdef DEBUG
  ~ChessType() { ObjectCounter::unregisterCT(); }
  #endif

  int length() const { return length_; }

  int level() const { return level_; }

  bool life() const { return life_; }

  bool operator==(const ChessType &type) const {
    return (length_ == type.length_ && life_ == type.life_ && level_ == type.level_);
  }

  bool operator!=(const ChessType &type) const {
    return (length_ != type.length_ || life_ != type.life_ || level_ != type.level_);
  }

  bool operator>(const ChessType &type) const {
    return ((length_ > type.length_) || ((length_ == type.length_) && (life_ > type.life_)));
  }

  bool operator<(const ChessType &type) const {
    return ((length_ < type.length_) || ((length_ == type.length_) && (life_ < type.life_)));
  }

  bool operator>=(const ChessType &type) const {
    return ((length_ > type.length_) || ((length_ == type.length_) && (life_ >= type.life_)));
  }

  bool operator<=(const ChessType &type) const {
    return ((length_ < type.length_) || ((length_ == type.length_) && (life_ <= type.life_)));
  }

 private:
  int length_, level_;
  bool life_;
};

#endif