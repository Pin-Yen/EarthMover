#ifndef GOMOKU_FREESTYLE_EVALUATOR_H
#define GOMOKU_FREESTYLE_EVALUATOR_H

class VirtualBoardFreeStyle::EvaluatorFreeStyle : public VirtualBoardGomoku::Evaluator {

 public:
  /* check win lor lose (call by virtualboard::play)
   * -1 for lose, 0 for nothng, 1 for win */
  static int checkWinOrLose(int score) { return score >= SCORE_WIN; }

  /* inputs an array of ChessTypes of both players in type[4][2],
   * returns black's score in score[0], white's in score[1] */
  static void evaluateScore(ChessType* type[4][2], int *score);
};

#endif