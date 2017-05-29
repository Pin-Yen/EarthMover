#ifndef GOMOKU_RENJU_BASIC_EVALUATOR_RENJU_BASIC_H
#define GOMOKU_RENJU_BASIC_EVALUATOR_RENJU_BASIC_H
class VirtualBoardRenjuBasic::EvaluatorRenjuBasic : public VirtualBoardGomoku::Evaluator {
 public:

  /* check win lor lose (call by virtualboard::play)
   * -1 for lose, 0 for nothng, 1 for win */
  static int checkWinOrLose(int score) {
    if (score >= SCORE_WIN) return 1;
    if (score == SCORE_FORBIDDEN) return -1;
    return 0;
  }

 private:
  class TypeTreeRenjuBasic;
};

#include "typetreerenjubasic.hpp"
void VirtualBoardRenjuBasic::EvaluatorRenjuBasic::init() {
  OpeningTree::init();
  TypeTreeRenjuBasic::init();
}


#endif