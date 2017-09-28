#ifndef GOMOKU_EVALUATOR_H_
#define GOMOKU_EVALUATOR_H_

#include "virtualboardgomoku.h"

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Evaluator {
 public:
  static void evaluateRelativeScore(VirtualBoardGomoku::Point point[LENGTH],
                                    int playNo);

  static const int SCORE_WIN = 10000000, SCORE_FORBIDDEN = -100;

 protected:
  template <class derivedTypeTree>
  class TypeTree;

  class OpeningTree;
};

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::evaluateRelativeScore(
    VirtualBoardGomoku<StatusLength>::Point point[LENGTH], int playNo) {
  if (playNo == 0) {
    for (int i = 0; i < LENGTH; ++i)
      point[i].setScore(-1);

    point[LENGTH / 2].setScore(1);
  } else {
    // using opening
    if (playNo <= 4) {
      int index = OpeningTree::classify(point);

      if (index != -1) {
        for (int i = 0; i < LENGTH; ++i)
            point[i].setScore(-1);

        point[index].setScore(1);
        return;
      }
    }

    // using absloute score

    bool whoTurn = playNo & 1;

    // get highest score
    int highestScore = -1;
    for (int i = 0; i < LENGTH; ++i)
      if (point[i].absScore(whoTurn) > highestScore)
        highestScore = point[i].absScore(whoTurn);

    for (int i = 0; i < LENGTH; ++i) {
      int score = point[i].absScore(whoTurn);
      if (score * 8 <= highestScore || (playNo < 10 && score < 140))
        point[i].setScore(-1);
      else
        point[i].setScore(score);
    }
  }
}

#endif  // GOMOKU_EVALUATOR_H_
