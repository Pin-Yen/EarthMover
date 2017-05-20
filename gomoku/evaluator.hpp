#ifndef GOMOKU_EVALUATOR_H
#define GOMOKU_EVALUATOR_H

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Evaluator {

 public:
  static void init();

  /* inputs status array in *status.
   * analyzes both player's chesstype, and stores black's type in type[0], white's in type[1] */
  static void evaluateType(STATUS *status, ChessType* type[2]);

  static void evaluateRelativeScore(VirtualBoardGomoku::Point* point[15][15], int playNo);

  static const int SCORE_WIN = 10000000;

 private:
  // class TypeTree;
  class OpeningTree;

  static bool isInit;
};

#include "chesstype.hpp"
#include "status.hpp"
#include "../virtualboard.hpp"
#include "virtualboardgomoku.hpp"
#include "point.hpp"
#include "openingtree.hpp"

template <int StatusLength>
bool VirtualBoardGomoku<StatusLength>::Evaluator::isInit = false;

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::init() {
  if (isInit) return;
  isInit = true;

  OpeningTree::init();
}

// template <int StatusLength>
// void VirtualBoardGomoku<StatusLength>::Evaluator::evaluateType(STATUS *status, ChessType* type[2]) {
//   TypeTree::classify(status, type);
// }

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::evaluateRelativeScore(
    VirtualBoardGomoku<StatusLength>::Point* point[15][15], int playNo) {
  if (playNo == 0) {
    for (int r = 0; r < 15; ++r)
      for (int c = 0; c < 15; ++c)
        point[r][c]->setRelScore(-1);

    point[7][7]->setRelScore(1);
  } else {
    /* using opening */
    if (playNo <= 4) {
      int row = -1, col = -1;
      OpeningTree::classify(point, &row, &col);

      if (row != -1) {
        for (int r = 0; r < 15; ++r) {
          for (int c = 0; c < 15; ++c) {
            if (r == row && c == col)
              point[r][c]->setRelScore(1);
            else
              point[r][c]->setRelScore(-1);
          }
        }
        return;
      }
    }

    /* using absloute score */

    bool whoTurn = playNo & 1;

    /* get highest score */
    int highestScore = -1;
    for (int r = 0; r < 15; ++r)
      for (int c = 0; c < 15; ++c)
        if (point[r][c]->absScore(whoTurn) > highestScore)
          highestScore = point[r][c]->absScore(whoTurn);

    for (int r = 0; r < 15; ++r)
      for (int c = 0; c < 15; ++c) {
        int score = point[r][c]->absScore(whoTurn);
        if (score * 8 < highestScore || (playNo < 10 && score < 140))
          point[r][c]->setRelScore(-1);
        else
          point[r][c]->setRelScore(score);
      }
  }
}

#endif