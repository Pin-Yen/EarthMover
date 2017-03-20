#ifndef EVALUATOR_H
#define EVALUATOR_H

class Evaluator {
 public:
  /* inputs status array in *status.
   * analyzes both player's chesstype, and stores black's type in type[0], white's in type[1] */
  static void evaluate_type(STATUS *status, ChessType* type[2]);

  /* inputs an array of ChessTypes of both players in type[4][2],
   * returns black's score in score[0], white's in score[1] */
  static void evaluate_score(ChessType* type[4][2], int *score);
};

#endif