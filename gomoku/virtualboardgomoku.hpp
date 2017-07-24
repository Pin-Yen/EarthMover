#ifndef GOMOKU_VIRTUAL_BOARD_H
#define GOMOKU_VIRTUAL_BOARD_H

#include "../virtualboard.hpp"

template <int StatusLength>
class VirtualBoardGomoku : public VirtualBoard {
 public:
  static const int CHESSBOARD_DIMEN = 15;

  VirtualBoardGomoku() {}
  // copy the source board to consturct the board
  VirtualBoardGomoku(VirtualBoardGomoku* source);

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

  // get score at "index"
  int getScore(int index) const final override;

  // get the sume of every point's score
  int getScoreSum() const final override;

  // get the highest score's index, return -1 if no useful point
  int getHSI() const final override;

  // get the highset score's index
  // only check if the corresponding "ignoreIndex[index]" is false
  int getHSI(bool ignoreIndex[]) const final override;

  // get who turn, black = 0, white = 1
  bool whoTurn() const final override { return (playNo_ & 1); }

  // pass
  void pass() { ++playNo_; }

  // point array
  Point* point_[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

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
template <class Eva>
void VirtualBoardGomoku<StatusLength>::init() {
  isInit_ = true;

  // initialize point array
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point_[r][c] = new Point();

  // index: 0→ 1↓ 2↗ 3↘
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      // set each point's status array pointer
      for (int d = 0; d < 4; ++d)
        for (int offset = -(StatusLength / 2), index = 0; offset <= (StatusLength / 2); ++offset) {
          if (offset == 0) continue;

          const int checkRow = r + dir[d][0] * offset,
                    checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
              checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
            // if out of bound, set pointer to NULL
            point_[r][c]->setDirStatus(d, index, NULL);
          else
            point_[r][c]->setDirStatus(d, index, point_[checkRow][checkCol]->statusRef());

          ++index;
        }

  playNo_ = 0;

  // evaluate
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      for (int d = 0; d < 4; ++d) {
        // get status array
        STATUS status[StatusLength]; point_[r][c]->getDirStatus(d, status);

        point_[r][c]->type[d] = Eva::evaluateType(status);
      }
      Eva::evaluateScore(point_[r][c]->type, point_[r][c]->absScore());
    }
  Eva::evaluateRelativeScore(point_, playNo_);

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::VirtualBoardGomoku(VirtualBoardGomoku<StatusLength>* source) {
  isInit_ = true;

  // copy point
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point_[r][c] = new Point(source->point_[r][c]);

  // index: 0→ 1↓ 2↗ 3↘
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      // set each point's status array pointer
      for (int d = 0; d < 4; ++d)
        for (int offset = -(StatusLength / 2), index = 0; offset <= (StatusLength / 2); ++offset) {

          if (offset == 0) continue;

          const int checkRow = r + dir[d][0] * offset,
                    checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
              checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
            // if out of bound, set pointer to NULL
            point_[r][c]->setDirStatus(d, index, NULL);
          else
            point_[r][c]->setDirStatus(d, index, point_[checkRow][checkCol]->statusRef());

          ++index;
        }

  playNo_ = source->playNo_;

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
}

template <int StatusLength>
VirtualBoardGomoku<StatusLength>::~VirtualBoardGomoku() {
  if (!isInit_) return;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      delete point_[r][c];

  #ifdef DEBUG
  ObjectCounter::unregisterVB();
  #endif
}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getScore(int index) const {
  return point_[0][index]->score();
}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getScoreSum() const {
  int sum = 0;
  int score;
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      // only add the score if it was positive
      if ((score = point_[r][c]->score()) > 0)
        sum += score;

  return sum;
}

template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::getHSI() const {
  // current max score, current same score amount
  int max = 0, same = 0, index = -1;

  for (int i = 0; i < CHESSBOARD_DIMEN * CHESSBOARD_DIMEN; ++i) {
    if (point_[0][i]->status() != EMPTY) continue;

    int score = point_[0][i]->score();

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

  for (int i = 0; i < CHESSBOARD_DIMEN * CHESSBOARD_DIMEN; ++i) {
    if (ignoreIndex[i]) continue;

    int score = point_[0][i]->score();

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
  int winOrLose = Eva::checkWinOrLose(point_[0][index]->absScore(playNo_ & 1));
  if (winOrLose != 0) return winOrLose;

  ++playNo_;

  STATUS color = ((playNo_ & 1) ? BLACK : WHITE);

  point_[0][index]->setStatus(color);

  // set score to -1
  point_[0][index]->setAbsScore(-1, -1);

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
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        // check if block
        if (point_[checkRow][checkCol]->status() != EMPTY) {
          block[point_[checkRow][checkCol]->status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        // get status array
        STATUS status[StatusLength]; point_[checkRow][checkCol]->getDirStatus(d, status);

        point_[checkRow][checkCol]->type[d] = Eva::evaluateType(status);

        Eva::evaluateScore(point_[checkRow][checkCol]->type,
                                 point_[checkRow][checkCol]->absScore());
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
  if (index == CHESSBOARD_DIMEN * CHESSBOARD_DIMEN) return;

  point_[0][index]->setStatus(EMPTY);

  for (int d = 0; d < 4; ++d) {
    // get status array
    STATUS status[StatusLength]; point_[0][index]->getDirStatus(d, status);

    point_[0][index]->type[d] = Eva::evaluateType(status);

    Eva::evaluateScore(point_[0][index]->type,
                       point_[0][index]->absScore());
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
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        // check if block
        if (point_[checkRow][checkCol]->status() != EMPTY) {
          block[point_[checkRow][checkCol]->status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        // get status array
        STATUS status[StatusLength]; point_[checkRow][checkCol]->getDirStatus(d, status);

        point_[checkRow][checkCol]->type[d] = Eva::evaluateType(status);

        Eva::evaluateScore(point_[checkRow][checkCol]->type,
                                 point_[checkRow][checkCol]->absScore());
      }
    }

  Eva::evaluateRelativeScore(point_, playNo_);
}

#endif