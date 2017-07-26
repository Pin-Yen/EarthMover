#ifndef GOMOKU_VIRTUAL_BOARD_H
#define GOMOKU_VIRTUAL_BOARD_H

#include "../virtualboard.hpp"

template <int StatusLength>
class VirtualBoardGomoku : public VirtualBoard {
 public:
  VirtualBoardGomoku() {}
  // copy constructor
  VirtualBoardGomoku(const VirtualBoardGomoku& source);

  ~VirtualBoardGomoku() override;

 protected:
  class Evaluator;

  template <class Eva>
  void init();

  // puts a new chess at "index",
  // returns 1 if wins after play, -1 if lose
  template <class Eva>
  int play(int index);

  // remove chess at "index"
  template <class Eva>
  void undo(int index);

 private:
  class Point;

  static const int DIMEN = 15, LENGTH = 225;

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

  // point array
  Point point_[LENGTH];

  // the total number of plays
  int playNo_;
};

#include "chesstype.hpp"
#include "status.hpp"
#include "../virtualboard.hpp"
#include "evaluator.hpp"
#include "point.hpp"

#include <random>
#include <iostream>

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

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
        for (int offset = -(StatusLength / 2), index = 0; offset <= (StatusLength / 2); ++offset) {
          if (offset == 0) continue;

          const int checkRow = r + dir[d][0] * offset,
                    checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= DIMEN ||
              checkCol < 0 || checkCol >= DIMEN)
            // if out of bound, set pointer to NULL
            point_[i].setDirStatus(d, index, NULL);
          else
            point_[i].setDirStatus(d, index, point_[checkRow * DIMEN + checkCol].statusRef());

          ++index;
        }

  playNo_ = source.playNo_;

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
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
        for (int offset = -(StatusLength / 2), index = 0; offset <= (StatusLength / 2); ++offset) {
          if (offset == 0) continue;

          const int checkRow = r + dir[d][0] * offset,
                    checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= DIMEN ||
              checkCol < 0 || checkCol >= DIMEN)
            // if out of bound, set pointer to NULL
            point_[i].setDirStatus(d, index, NULL);
          else
            point_[i].setDirStatus(d, index, point_[checkRow * DIMEN + checkCol].statusRef());

          ++index;
        }

  playNo_ = 0;

  // evaluate
  for (int i = 0; i < LENGTH; ++i) {
    for (int d = 0; d < 4; ++d) {
        // get status array
      STATUS status[StatusLength];
      point_[i].getDirStatus(d, status);

      point_[i].type(d) = Eva::evaluateType(status);
    }
    Eva::evaluateScore(point_[i].type(), point_[i].absScore());
  }
  Eva::evaluateRelativeScore(point_, playNo_);

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::~VirtualBoardGomoku() {
  #ifdef DEBUG
  ObjectCounter::unregisterVB();
  #endif
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
      if (((double)rand() / RAND_MAX) <= (1. / same)) {
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
int VirtualBoardGomoku<StatusLength>::play(int index) {
  int winOrLose = Eva::checkWinOrLose(point_[index].absScore(playNo_ & 1));
  if (winOrLose != 0) return winOrLose;

  ++playNo_;

  STATUS color = ((playNo_ & 1) ? BLACK : WHITE);

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
        if (checkRow < 0 || checkRow >= DIMEN ||
            checkCol < 0 || checkCol >= DIMEN) break;

        const int checkIndex = checkRow * DIMEN + checkCol;

        // check if block
        if (point_[checkIndex].status() != EMPTY) {
          block[point_[checkIndex].status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        // get status array
        STATUS status[StatusLength];
        point_[checkIndex].getDirStatus(d, status);

        point_[checkIndex].type(d) = Eva::evaluateType(status);

        Eva::evaluateScore(point_[checkIndex].type(),
                           point_[checkIndex].absScore());
      }
    }

  Eva::evaluateRelativeScore(point_, playNo_);

  return 0;
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
    STATUS status[StatusLength];
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
        if (checkRow < 0 || checkRow >= DIMEN ||
            checkCol < 0 || checkCol >= DIMEN) break;

        const int checkIndex = checkRow * DIMEN + checkCol;

        // check if block
        if (point_[checkIndex].status() != EMPTY) {
          block[point_[checkIndex].status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        // get status array
        STATUS status[StatusLength];
        point_[checkIndex].getDirStatus(d, status);

        point_[checkIndex].type(d) = Eva::evaluateType(status);

        Eva::evaluateScore(point_[checkIndex].type(),
                           point_[checkIndex].absScore());
      }
    }

  Eva::evaluateRelativeScore(point_, playNo_);
}

#endif
