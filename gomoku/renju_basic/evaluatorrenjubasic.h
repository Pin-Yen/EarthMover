#ifndef GOMOKU_RENJU_BASIC_EVALUATORRENJUBASIC_H_
#define GOMOKU_RENJU_BASIC_EVALUATORRENJUBASIC_H_

class VirtualBoardRenjuBasic::EvaluatorRenjuBasic :
    public VirtualBoardGomoku::Evaluator {
 public:
  static inline void init();

  // check win lor lose (call by virtualboard::play)
  // -1 for lose, 0 for nothng, 1 for win
  static int checkWinOrLose(int score) {
    if (score >= SCORE_WIN) return 1;
    if (score == SCORE_FORBIDDEN) return -1;
    return 0;
  }

  // inputs status array in *status.
  // analyzes both player's chesstype, and return it
  static inline ChessType evaluateType(const STATUS *status);

  // inputs an array of ChessTypes in type[4],
  // returns black's score in score[0], white's in score[1]
  static void evaluateScore(ChessType type[4], int *score);

 private:
  class TypeTreeRenjuBasic;
};

#include "typetreerenjubasic.h"

void VirtualBoardRenjuBasic::EvaluatorRenjuBasic::init() {
  OpeningTree::init();
  TypeTreeRenjuBasic::init();
}

ChessType VirtualBoardRenjuBasic::EvaluatorRenjuBasic::evaluateType(
    const STATUS *status) {
  return TypeTreeRenjuBasic::classify(status);
}

#endif  // GOMOKU_RENJU_BASIC_EVALUATORRENJUBASIC_H_
