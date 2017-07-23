#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

class ChessType {
 public:
  // default constructor
  ChessType(): length_(0), level_(0), life_(0) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  ChessType(char length, char life, char level): length_(length), level_(level), life_(life) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  // copy constructor
  ChessType(const ChessType& source) {
    length_ = source.length_;
    life_ = source.life_;
    level_ = source.level_;

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

  // getter
  char length() const { return length_; }
  char life() const { return life_; }
  char level() const { return level_; }

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
  char length_, life_, level_;
};

#endif