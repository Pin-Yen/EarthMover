#ifndef GOMOKU_POINT_H_
#define GOMOKU_POINT_H_

#include "chesstype.h"
#include "virtualboardgomoku.h"

#include <cstddef>

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Point {
 public:
  Point() : status_(EMPTY) {}

  Point(const Point& source)
      : status_(source.status_),
        score_(source.score_),
        absScore_{ source.absScore_[0], source.absScore_[1] },
        type_{ source.type_[0], source.type_[1],
               source.type_[2], source.type_[3] } {}

  ~Point() {}

  // Set the status pointer.
  // Note: to finish initialize this point,
  // shound call this to write all status pointer
  void setDirStatus(int dir, int index, const StoneStatus* status) {
    dirStatus_[dir][index] = status;
  }

  // get the status by diraction, copy the pointer's value to dest
  void getDirStatus(int dir, StoneStatus* dest) const {
    for (int i = 0; i < StatusLength; ++i)
      dest[i] = (dirStatus_[dir][i] == NULL ? BOUND : *(dirStatus_[dir][i]));
  }

  void setStatus(StoneStatus status) { status_ = status; }
  StoneStatus status() const { return status_; }
  const StoneStatus* statusRef() { return &status_; }

  int* absScore() { return absScore_; }
  int absScore(bool color) const { return absScore_[color]; }

  int score() const { return score_; }
  void setScore(int score) { score_ = score; }

  void setAbsScore(int black, int white) {
    absScore_[0] = black;
    absScore_[1] = white;
  }

  ChessType& type(int index) { return type_[index]; }
  ChessType* type() { return type_; }

 private:
  // chess type array
  // index: 0→ 1↓ 2↗ 3↘
  ChessType type_[4];

  // StoneStatus array pointer, this will point at other point's Status color
  // index: 0→ 1↓ 2↗ 3↘
  const StoneStatus* dirStatus_[4][StatusLength];

  // point's status, the target of the other point's StoneStatus array pointer
  StoneStatus status_;

  int absScore_[2];
  int score_;
};

#endif  // GOMOKU_POINT_H_
