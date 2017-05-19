#include "evaluator.hpp"
#include "../virtualboardgomoku.hpp"

VirtualBoardGomoku::Evaluator::isInitialized = false;

void VirtualBoardGomoku::Evaluator::evaluateRelativeScore(VirtualBoardGomoku::Point* point[15][15], int playNo) {
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
