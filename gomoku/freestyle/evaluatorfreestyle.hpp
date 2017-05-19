#ifndef EVALUATOR_FREESTYLE_H_
#define EVALUATOR_FREESTYLE_H_
#include "../chesstype.hpp"
#include "../status.hpp"

#include "typetree.hpp" // @Temporary

class VirtualBoardFreeStyle::EvaluatorFreeStyle : public VirtualBoardGomoku::Evaluator{
 public:
  static void initialize() override;

  static void evaluateType(STATUS *status, ChessType* type[2]) override;

  static evaluateScore(ChessType* type[4][2], int *score) override;

 private:
  // @Temporary
  class TypeTree;
};

#endif