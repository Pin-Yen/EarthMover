#ifndef GOMOKU_EVALUATOR_H
#define GOMOKU_EVALUATOR_H

#include "virtualboardgomoku.hpp"

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Evaluator {
 public:

  /*** MUST be implemented in derived class ***
   * initializes Evaluator */
  static void init();

  /*** MUST be implemented in derived class ***
   * analyzes both player's chesstype
   * params:
   *        status  : status array
   *        type[2] : pointer to a 2d array, storing result type. [0] for black's and [1] for white's
   */
  static void evaluateType(STATUS *status, ChessType* type[2]);

  /*** MUST be implemented in derived class ***
   * evaluate score of the given type
   * params:
            type[d][p]: typeArray in direction d, player p
   *        score     : a pointer to an array of size 2 to store results, [0] for black's and [1] for white's
   */
  static void evaluateScore(ChessType* type[4][2], int *score);

  /*** MUST be implemented in derived class ***
   * params:
   *        score: the score of the point to be checked
   * return value: -1 : lose.  0 : nothing. 1 : win.
   */
  static int checkWinOrLose(int score);

  static void evaluateRelativeScore(VirtualBoardGomoku::Point* point[15][15], int playNo);

  static const int SCORE_WIN = 10000000, SCORE_FORBIDDEN = -100;

 protected:

  template <class derivedTypeTree>
  class TypeTree;

  class OpeningTree;
};

#include "chesstype.hpp"
#include "status.hpp"
#include "../virtualboard.hpp"
#include "virtualboardgomoku.hpp"
#include "point.hpp"
#include "openingtree.hpp"

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