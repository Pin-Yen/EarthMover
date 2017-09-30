#ifndef GOMOKU_VIRTUALBOARDGOMOKU_H_
#define GOMOKU_VIRTUALBOARDGOMOKU_H_

#include <random>

#include "../virtualboard.h"

template <int StatusLength>
class VirtualBoardGomoku : public VirtualBoard {
 public:
  VirtualBoardGomoku() {}
  // copy constructor
  VirtualBoardGomoku(const VirtualBoardGomoku& source);

  ~VirtualBoardGomoku() override {};

 protected:
  class Evaluator;

  template <class Eva>
  void init();

  // puts a new chess at "index",
  // returns 1 if wins after play, -1 if lose
  template <class Eva>
  GameStatus play(int index);

  // remove chess at "index"
  template <class Eva>
  void undo(int index);

 private:
  class Point;

  static const int DIMEN = 15, LENGTH = 225;

  int length() const final { return LENGTH; }

  // get score at "index"
  inline int getScore(int index) const final;

  // get the sume of every point's score
  int getScoreSum() const final;

  // get the highest score's index, return -1 if no useful point
  int getHSI() const final;

  // get the highset score's index
  // only check if the corresponding "ignoreIndex[index]" is false
  int getHSI(bool ignoreIndex[]) const final;

  // get who turn, black = 0, white = 1
  bool whoTurn() const final { return (playNo_ & 1); }

  // pass
  // return the index of pass
  int pass() { ++playNo_; return LENGTH; }

  bool outOfBound(int r, int c) {
    return r < 0 || r >= DIMEN || c < 0 || c >= DIMEN;
  }

  // point array
  Point point_[LENGTH];

  // the total number of plays
  int playNo_;
};

#include "chesstype.h"
#include "point.h"

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::VirtualBoardGomoku(
    const VirtualBoardGomoku<StatusLength>& source) {
  isInit_ = true;

  // copy point
  for (int i = 0; i < LENGTH; ++i)
    point_[i] = Point(source.point_[i]);

  // index: 0→ 1↓ 2↗ 3↘
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0, i = 0; r < DIMEN; ++r)
    for (int c = 0; c < DIMEN; ++c, ++i)
      // set each point's status array pointer
      for (int d = 0; d < 4; ++d)
        for (int offset = -(StatusLength / 2), index = 0;
             offset <= (StatusLength / 2); ++offset) {
          if (offset == 0) continue;

          const int checkRow = r + dir[d][0] * offset,
                    checkCol = c + dir[d][1] * offset;

          if (outOfBound(checkRow, checkCol)) {
            // if out of bound, set pointer to NULL
            point_[i].setDirStatus(d, index, NULL);
          } else {
            point_[i].setDirStatus(
                d, index, point_[checkRow * DIMEN + checkCol].statusRef());
          }

          ++index;
        }

  playNo_ = source.playNo_;
}

template <int StatusLength>
template <class Eva>
void VirtualBoardGomoku<StatusLength>::init() {
  isInit_ = true;

  // index: 0→ 1↓ 2↗ 3↘
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  // "i" is for point index
  for (int r = 0, i = 0; r < DIMEN; ++r)
    for (int c = 0; c < DIMEN; ++c, ++i)
      // set each point's status array pointer
      for (int d = 0; d < 4; ++d)
        for (int offset = -(StatusLength / 2), index = 0;
             offset <= (StatusLength / 2); ++offset) {
          if (offset == 0) continue;

          const int checkRow = r + dir[d][0] * offset,
                    checkCol = c + dir[d][1] * offset;

          if (outOfBound(checkRow, checkCol)) {
            // if out of bound, set pointer to NULL
            point_[i].setDirStatus(d, index, NULL);
          } else {
            point_[i].setDirStatus(
                d, index, point_[checkRow * DIMEN + checkCol].statusRef());
          }

          ++index;
        }

  playNo_ = 0;

  // evaluate
  for (int i = 0; i < LENGTH; ++i) {
    for (int d = 0; d < 4; ++d) {
        // get status array
      StoneStatus status[StatusLength];
      point_[i].getDirStatus(d, status);

      point_[i].type(d) = Eva::evaluateType(status);
    }
    Eva::evaluateScore(point_[i].type(), point_[i].absScore());
  }
  Eva::evaluateRelativeScore(point_, playNo_);

}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getScore(int index) const {
  return point_[index].score();
}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getScoreSum() const {
  int sum = 0;
  int score;
  for (int i = 0; i < LENGTH; ++i)
    // only add the score if it was positive
    if ((score = point_[i].score()) > 0)
      sum += score;

  return sum;
}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getHSI() const {
  // current max score, current same score amount
  int max = 0, same = 0, index = -1;

  for (int i = 0; i < LENGTH; ++i) {
    if (point_[i].status() != EMPTY) continue;

    int score = point_[i].score();

    if (score > max) {
      same = 1;

      max = score;
      index = i;
    } else if (score == max) {
      ++same;
      if ((static_cast<double>(rand()) / RAND_MAX) <= (1. / same)) {
        index = i;
      }
    }
  }

  // return -1 means that there is no useful point(can pass now)
  // Not representative of each point is occupied
  return index;
}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getHSI(bool ignoreIndex[]) const {
  // current max score, current same score amount
  int max = 0, index = -1;

  for (int i = 0; i < LENGTH; ++i) {
    if (ignoreIndex[i]) continue;

    int score = point_[i].score();

    if (score > max) {
      max = score;
      index = i;
    }
  }

  // return -1 means that there is no useful point
  // Not representative of each point is occupied
  return index;
}

template <int StatusLength>
template <class Eva>
GameStatus VirtualBoardGomoku<StatusLength>::play(int index) {
  GameStatus status = Eva::checkWinOrLose(point_[index].absScore(playNo_ & 1));
  if (status != NOTHING) return status;

  ++playNo_;

  StoneStatus color = ((playNo_ & 1) ? BLACK : WHITE);

  point_[index].setStatus(color);

  // set score to -1
  point_[index].setAbsScore(-1, -1);

  // index: 0→ 1↓ 2↗ 3↘
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};
  const int row = index / 15, col = index % 15;

  for (int d = 0; d < 4; ++d)
    for (int move = -1; move <= 1; move += 2) {
      bool block[2] = {false, false};

      for (int offset = 1; offset <= (StatusLength / 2) + 1; ++offset) {
        const int checkRow = row + dir[d][0] * move * offset,
                  checkCol = col + dir[d][1] * move * offset;

        // check if out the bound
        if (outOfBound(checkRow, checkCol)) break;

        const int checkIndex = checkRow * DIMEN + checkCol;

        // check if block
        if (point_[checkIndex].status() != EMPTY) {
          block[point_[checkIndex].status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        // get status array
        StoneStatus status[StatusLength];
        point_[checkIndex].getDirStatus(d, status);

        point_[checkIndex].type(d) = Eva::evaluateType(status);

        Eva::evaluateScore(point_[checkIndex].type(),
                           point_[checkIndex].absScore());
      }
    }

  Eva::evaluateRelativeScore(point_, playNo_);

  return NOTHING;
}

template <int StatusLength>
template <class Eva>
void VirtualBoardGomoku<StatusLength>::undo(int index) {
  --playNo_;

  // index == 225 means that previous move is pass
  if (index == DIMEN * DIMEN) return;

  point_[index].setStatus(EMPTY);

  for (int d = 0; d < 4; ++d) {
    // get status array
    StoneStatus status[StatusLength];
    point_[index].getDirStatus(d, status);

    point_[index].type(d) = Eva::evaluateType(status);

    Eva::evaluateScore(point_[index].type(), point_[index].absScore());
  }

  // index: 0→ 1↓ 2↗ 3↘
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};
  const int row = index / 15, col = index % 15;

  for (int d = 0; d < 4; ++d)
    for (int move = -1; move <= 1; move += 2) {
      bool block[2] = {false, false};

      for (int offset = 1; offset <= (StatusLength / 2) + 1; ++offset) {
        const int checkRow = row + dir[d][0] * move * offset,
                  checkCol = col + dir[d][1] * move * offset;

        // check if out the bound
        if (outOfBound(checkRow, checkCol)) break;

        const int checkIndex = checkRow * DIMEN + checkCol;

        // check if block
        if (point_[checkIndex].status() != EMPTY) {
          block[point_[checkIndex].status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        // get status array
        StoneStatus status[StatusLength];
        point_[checkIndex].getDirStatus(d, status);

        point_[checkIndex].type(d) = Eva::evaluateType(status);

        Eva::evaluateScore(point_[checkIndex].type(),
                           point_[checkIndex].absScore());
      }
    }

  Eva::evaluateRelativeScore(point_, playNo_);
}

#endif  // GOMOKU_VIRTUALBOARDGOMOKU_H_
