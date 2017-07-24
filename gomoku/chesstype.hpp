#ifndef CHESS_TYPE_H
#define CHESS_TYPE_H

#include <stdint.h>

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

class SingleType {
 public:
  // default constructor
  SingleType(): length_(0), life_(0), level_(0) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  SingleType(int8_t length, int8_t life, int8_t level): length_(length), life_(life), level_(level) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  // copy constructor
  SingleType(const SingleType& source): length_(source.length_), life_(source.life_), level_(source.level_) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  SingleType(const SingleType *source): length_(source->length_), life_(source->life_), level_(source->level_) {
    #ifdef DEBUG
    ObjectCounter::registerCT();
    #endif
  }

  #ifdef DEBUG
  ~SingleType() { ObjectCounter::unregisterCT(); }
  #endif

  // getter
  int8_t length() const { return length_; }
  int8_t life() const { return life_; }
  int8_t level() const { return level_; }

  bool operator==(const SingleType &type) const {
    return (length_ == type.length_ && life_ == type.life_ && level_ == type.level_);
  }

  bool operator!=(const SingleType &type) const {
    return (length_ != type.length_ || life_ != type.life_ || level_ != type.level_);
  }

  bool operator>(const SingleType &type) const {
    return ((length_ > type.length_) || ((length_ == type.length_) && (life_ > type.life_)));
  }

  bool operator<(const SingleType &type) const {
    return ((length_ < type.length_) || ((length_ == type.length_) && (life_ < type.life_)));
  }

  bool operator>=(const SingleType &type) const {
    return ((length_ > type.length_) || ((length_ == type.length_) && (life_ >= type.life_)));
  }

  bool operator<=(const SingleType &type) const {
    return ((length_ < type.length_) || ((length_ == type.length_) && (life_ <= type.life_)));
  }

 private:
  int8_t length_, life_, level_;
};

class ChessType {
 public:
  ChessType() {}

  ChessType(const ChessType& type) {
    type_[0] = SingleType(type.type_[0]);
    type_[1] = SingleType(type.type_[1]);
  }

  ChessType(const ChessType* type) {
    type_[0] = SingleType(type->type_[0]);
    type_[1] = SingleType(type->type_[1]);
  }

  ~ChessType() {}

  bool operator==(const ChessType &type) const {
    return (type_[0] == type.type_[0] && type_[1] == type.type_[1]);
  }

  bool operator!=(const ChessType &type) const {
    return (type_[0] != type.type_[0] || type_[1] != type.type_[1]);
  }

  SingleType type_[2];
};

#endif