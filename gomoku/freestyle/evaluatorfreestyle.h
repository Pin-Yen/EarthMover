#ifndef GOMOKU_FREESTYLE_EVALUATORFREESTYLE_H_
#define GOMOKU_FREESTYLE_EVALUATORFREESTYLE_H_

#include "../evaluator.h"
#include "../typetree.h"

class VirtualBoardFreeStyle::EvaluatorFreeStyle : public VirtualBoardGomoku::Evaluator {
 public:
  static inline void init();

  // check win lor lose (call by virtualboard::play)
  // -1 for lose, 0 for nothng, 1 for win
  static int checkWinOrLose(int score) { return score >= SCORE_WIN; }

  // inputs status array in *status.
  // analyzes both player's chesstype, and return it
  static inline ChessType evaluateType(const STATUS *status);

  // inputs an array of ChessTypes,
  // returns black's score in score[0], white's in score[1]
  static void evaluateScore(ChessType type[4], int *score);

 private:
  class TypeTreeFreeStyle;
};

#include "typetreefreestyle.h"

void VirtualBoardFreeStyle::EvaluatorFreeStyle::init() {
  OpeningTree::init();
  TypeTreeFreeStyle::init();
}

ChessType VirtualBoardFreeStyle::EvaluatorFreeStyle::evaluateType(const STATUS *status) {
  return TypeTreeFreeStyle::classify(status);
}

#endif
