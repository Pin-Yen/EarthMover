#ifndef GOMOKU_RENJU_BASIC_EVALUATORRENJUBASIC_H_
#define GOMOKU_RENJU_BASIC_EVALUATORRENJUBASIC_H_

#include "../evaluator.h"
#include "../openingtree.h"
#include "virtualboardrenjubasic.h"

class VirtualBoardRenjuBasic::EvaluatorRenjuBasic :
    public VirtualBoardGomoku::Evaluator {
 public:
  static void init();

  // check win lor lose (call by virtualboard::play)
  // -1 for lose, 0 for nothng, 1 for win
  static GameStatus checkWinOrLose(int score) {
    if (score >= SCORE_WIN) return WINNING;
    if (score == SCORE_FORBIDDEN) return LOSING;
    return NOTHING;
  }

  // inputs status array in *status.
  // analyzes both player's chesstype, and return it
  static ChessType evaluateType(const StoneStatus *status);

  // inputs an array of ChessTypes in type[4],
  // returns black's score in score[0], white's in score[1]
  static void evaluateScore(ChessType type[4], int *score);

 private:
  class TypeTreeRenjuBasic;
};

#endif  // GOMOKU_RENJU_BASIC_EVALUATORRENJUBASIC_H_
