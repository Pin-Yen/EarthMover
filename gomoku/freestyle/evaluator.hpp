#ifndef GOMOKU_FREESTYLE_EVALUATOR_H
#define GOMOKU_FREESTYLE_EVALUATOR_H

class VirtualBoardGomoku::Evaluator {
 public:
  static void initialize();

  /* check win lor lose (call by virtualboard::play)
   * -1 for lose, 0 for nothng, 1 for win */
  static int checkWinOrLose(int score) { return score >= SCORE_WIN; }

  /* inputs status array in *status.
   * analyzes both player's chesstype, and stores black's type in type[0], white's in type[1] */
  static void evaluateType(STATUS *status, ChessType* type[2]);

  /* inputs an array of ChessTypes of both players in type[4][2],
   * returns black's score in score[0], white's in score[1] */
  static void evaluateScore(ChessType* type[4][2], int *score);

  static void evaluateRelativeScore(VirtualBoardGomoku::Point* point[15][15], int playNo);

  static const int SCORE_WIN = 10000000;

 private:
  class TypeTree;
  class OpeningTree;

  static bool isInitialized;
};

#endif