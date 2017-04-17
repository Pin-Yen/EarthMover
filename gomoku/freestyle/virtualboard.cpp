#include "../chesstype.hpp"
#include "../status.hpp"
#include "virtualboard.hpp"
#include "point.hpp"

#include <random>
#include <iostream>

#include "evaluator.hpp"

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

VirtualBoard::VirtualBoard() {
  Evaluator::initialize();

  /* initialize point array */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point_[r][c] = new Point();

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* set each point's status array pointer */
      for (int d = 0; d < 4; ++d)
        for (int offset = -4, index = 0; offset <= 4; ++offset) {
          if (offset == 0) continue;

          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
            /* if out of bound, set pointer to NULL */
            point_[r][c]->setDirStatus(d, index, NULL);
          else
            point_[r][c]->setDirStatus(d, index, point_[checkRow][checkCol]->statusRef());

          ++index;
        }

  playNo_ = 0;

  /* evaluate */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      for (int d = 0; d < 4; ++d) {
        /* get status array */
        STATUS status[8]; point_[r][c]->getDirStatus(d, status);

        Evaluator::evaluateType(status, point_[r][c]->type[d]);
      }
      Evaluator::evaluateScore(point_[r][c]->type, point_[r][c]->absScore());
    }
  Evaluator::evaluateRelativeScore(point_, playNo_);

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
}

VirtualBoard::VirtualBoard(VirtualBoard* source) {
  /* copy point */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point_[r][c] = new Point(source->point_[r][c]);

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* set each point's status array pointer */
      for (int d = 0; d < 4; ++d)
        for (int offset = -4, index = 0; offset <= 4; ++offset) {

          if (offset == 0) continue;

          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
            /* if out of bound, set pointer to NULL */
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

VirtualBoard::~VirtualBoard() {
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      delete point_[r][c];

  #ifdef DEBUG
  ObjectCounter::unregisterVB();
  #endif
}

int VirtualBoard::getScoreSum() {
  int sum = 0;
  int score;
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* only add the score if it was positive */
      if ((score = point_[r][c]->getScore()) > 0)
        sum += score;

  return sum;
}

bool VirtualBoard::getHSP(int &row, int &col) {
  /* current max score, current same score amount */
  int max = 0, same = 0;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      if (point_[r][c]->status() != EMPTY) continue;

      int score = point_[r][c]->getScore();

      if (score > max) {
        same = 1;

        max = score;
        row = r; col = c;
      } else if (score == max) {
        ++same;
        if (((double)rand() / RAND_MAX) <= (1. / same)) {
          row = r; col = c;
        }
      }
    }

  /* if every point has chess, then return false */
  return (max > -1);
}

bool VirtualBoard::play(int row, int col) {
  int score = point_[row][col]->getScore();
  if (score == 10000000) return true;

  ++playNo_;

  STATUS color = ((playNo_ & 1) ? BLACK : WHITE);

  point_[row][col]->play(color);

  /* set score to -1 */
  point_[row][col]->setScore(-1, -1);

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int d = 0; d < 4; ++d)
    for (int move = -1; move <= 1; move += 2) {
      bool block[2] = {false, false};

      for (int offset = 1; offset <= 5; ++offset) {
        int checkRow = row + dir[d][0] * move * offset,
          checkCol = col + dir[d][1] * move * offset;

        /* check if out the bound */
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        /* check if block */
        if (point_[checkRow][checkCol]->status() != EMPTY) {
          block[point_[checkRow][checkCol]->status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        /* get status array */
        STATUS status[8]; point_[checkRow][checkCol]->getDirStatus(d, status);

        Evaluator::evaluateType(status, point_[checkRow][checkCol]->type[d]);

        Evaluator::evaluateScore(point_[checkRow][checkCol]->type,
                                 point_[checkRow][checkCol]->absScore());
      }
    }

  Evaluator::evaluateRelativeScore(point_, playNo_ & 1, score);

  return false;
}