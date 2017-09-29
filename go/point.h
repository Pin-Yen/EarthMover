#ifndef GO_POINT_H_
#define GO_POINT_H_

#include "virtualboardgo.h"

class VirtualBoardGo::Point {
 public:
  Point() : status_(EMPTY) {}

  Point(const Point &src) : status_(src.status_) {}

 private:
  GameStatus status_;
};

#endif  // GO_POINT_H_
