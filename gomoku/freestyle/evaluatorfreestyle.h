#ifndef GOMOKU_FREESTYLE_EVALUATORFREESTYLE_H_
#define GOMOKU_FREESTYLE_EVALUATORFREESTYLE_H_

#include "../evaluator.h"
#include "../openingtree.h"
#include "virtualboardfreestyle.h"

class VirtualBoardFreeStyle::EvaluatorFreeStyle :
    public VirtualBoardGomoku::Evaluator {
 public:
  static void init();

  // check win lor lose (call by virtualboard::play)
  // -1 for lose, 0 for nothng, 1 for win
  static GameStatus checkWinOrLose(int score) {
    return score >= SCORE_WIN ? WINNING : NOTHING;
  }

  // inputs status array in *status.
  // analyzes both player's chesstype, and return it
  static ChessType evaluateType(const STATUS *status);

  // inputs an array of ChessTypes,
  // returns black's score in score[0], white's in score[1]
  static void evaluateScore(ChessType type[4], int *score);

 private:
  class TypeTreeFreeStyle;
};

#endif  // GOMOKU_FREESTYLE_EVALUATORFREESTYLE_H_
