#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#include <stdint.h>

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

class ChessType {
 public:
  // default constructor
  ChessType(): length_(0), life_(0), level_(0) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  ChessType(int8_t length, int8_t life, int8_t level): length_(length), life_(life), level_(level) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  // copy constructor
  ChessType(const ChessType& source): length_(source.length_), life_(source.life_), level_(source.level_) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  ChessType(const ChessType *source): length_(source->length_), life_(source->life_), level_(source->level_) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  #ifdef DEBUG
  ~ChessType() { ObjectCounter::unregisterCT(); }
  #endif

  // getter
  int8_t length() const { return length_; }
  int8_t life() const { return life_; }
  int8_t level() const { return level_; }

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
  int8_t length_, life_, level_;
};

#endif