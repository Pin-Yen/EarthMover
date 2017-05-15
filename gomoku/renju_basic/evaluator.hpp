#ifndef EVALUATOR_H
#define EVALUATOR_H

class VirtualBoard::Evaluator {
 public:
  static void initialize();

  /* inputs status array in *status.
   * analyzes both player's chesstype, and stores black's type in type[0], white's in type[1] */
  static void evaluateType(STATUS *status, ChessType* type[2]);

  /* inputs an array of ChessTypes of both players in type[4][2],
   * returns black's score in score[0], white's in score[1] */
  static void evaluateScore(ChessType* type[4][2], int *score);

  static void evaluateRelativeScore(VirtualBoard::Point* point[15][15], int playNo);

  static const int SCORE_WIN = 10000000, SCORE_FORBIDDEN = -100;

 private:
  class TypeTree;
  class OpeningTree;

  static bool isInitialized;
};

#endif