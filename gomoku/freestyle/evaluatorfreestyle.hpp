#ifndef GOMOKU_FREESTYLE_EVALUATOR_H
#define GOMOKU_FREESTYLE_EVALUATOR_H
#include "../evaluator.hpp"
#include "../typetree.hpp"
class VirtualBoardFreeStyle::EvaluatorFreeStyle : public VirtualBoardGomoku::Evaluator {

 public:

  /* check win lor lose (call by virtualboard::play)
   * -1 for lose, 0 for nothng, 1 for win */
  static int checkWinOrLose(int score) { return score >= SCORE_WIN; }

 private:
  class TypeTreeFreeStyle;
};

#include "typetreefreestyle.hpp"

inline void VirtualBoardFreeStyle::EvaluatorFreeStyle::init() {
  OpeningTree::init();
  TypeTreeFreeStyle::init();
}

#endif