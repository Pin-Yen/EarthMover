#include "chesstype.hpp"
#include "status.hpp"
#include "virtualboard.hpp"
#include "point.hpp"

#include <random>
#include <iostream>

#include "evaluator.hpp"

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

VirtualBoard::VirtualBoard() {
  /* initialize point array */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point();

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* set each poit's status array pointer*/
      for (int d = 0; d < 4; ++d)
        for (int offset = -4, index = 0; offset <= 4; ++offset) {

          if (offset == 0) continue;

          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
            /* if out of bound, set pointer value to bound */
            point[r][c]->setDirStatus(d, index, NULL);
          else
            point[r][c]->setDirStatus(d, index, point[checkRow][checkCol]->statusRef());

          ++index;
        }

  playNo = 0;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      for (int d = 0; d < 4; ++d) {
        /* get status array */
        STATUS status[8]; point[r][c]->getDirStatus(d, status);

        Evaluator::evaluateType(status, point[r][c]->type[d]);
      }

      //Evaluator::evaluateScore(point[r][c]->type, score[r][c]);
      Evaluator::evaluateScore(point[r][c]->type, point[r][c]->absScore());
    }

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
}

VirtualBoard::VirtualBoard(VirtualBoard* source) {
  /* copy point */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(source->point[r][c]);

  /* copy score */
  /*for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      for (int color = 0; color < 2; ++color)
        score[r][c][color] = source->score[r][c][color];*/

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* set each point's status array pointer*/
      for (int d = 0; d < 4; ++d)
        for (int offset = -4, index = 0; offset <= 4; ++offset) {

          if (offset == 0) continue;

          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN ||
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
            /* if out of bound, set pointer value to bound */
            point[r][c]->setDirStatus(d, index, NULL);
          else
            point[r][c]->setDirStatus(d, index, point[checkRow][checkCol]->statusRef());

          ++index;
        }

  playNo = source->playNo;

  #ifdef DEBUG
  ObjectCounter::registerVB();
  #endif
}

VirtualBoard::~VirtualBoard() {
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      delete point[r][c];

  #ifdef DEBUG
  ObjectCounter::unregisterVB();
  #endif
}

int VirtualBoard::getScoreSum() {
  bool color = whoTurn();
  int sum = 0;
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      //sum += score[r][c][color];
      sum += point[r][c]->getScore(color);

  return sum;
}

bool VirtualBoard::getHSP(int &row, int &col) {
  bool color = whoTurn();
  int max = 0, same = 0;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      if (point[r][c]->status() != EMPTY) continue;

      int score = point[r][c]->getScore(color);

      if (score/*[r][c][color]*/ > max) {
        same = 1;

        max = score/*[r][c][color]*/;
        row = r; col = c;
      }
      else if (score/*[r][c][color]*/ == max) {
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
  ++playNo;

  //if (score[row][col][(playNo - 1) & 1] == 10000000) return true;
  if (point[row][col]->getScore((playNo - 1) & 1) == 10000000) return true;

  STATUS color = ((playNo & 1) ? BLACK : WHITE);

  /* set score to -1 */
  //score[row][col][0] = -1; score[row][col][1] = -1;
  point[row][col]->setScore(-1, -1);

  point[row][col]->play(color);

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

        if (point[checkRow][checkCol]->status() != EMPTY) {
          block[point[checkRow][checkCol]->status()] = true;

          if (block[0] & block[1]) break;

          continue;
        }

        /* get status array */
        STATUS status[8]; point[checkRow][checkCol]->getDirStatus(d, status);

        Evaluator::evaluateType(status, point[checkRow][checkCol]->type[d]);

        //Evaluator::evaluateScore(point[checkRow][checkCol]->type,
        //                         score[checkRow][checkCol]);
        Evaluator::evaluateScore(point[checkRow][checkCol]->type,
                                 point[checkRow][checkCol]->absScore());
      }
    }

  return false;
}