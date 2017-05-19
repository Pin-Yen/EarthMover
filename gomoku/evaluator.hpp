#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include "chesstype.hpp"
#include "openingtree.hpp"
#include "status.hpp"
#include "virtualboardgomoku.hpp"

class VirtualBoard::Evaluator
{
public:

  /* initialize Evaluator */
  static virtual void initialize() = 0;

  /* check win lor lose
   * -1 for lose, 0 for nothng, 1 for win */
  static int checkWinOrLose(int score) {return score >= SCORE_WIN }

  /* inputs status array in *status.
   * analyzes both player's chesstype, and stores black's type in type[0], white's in type[1] */
  static virtual void evaluateType(STATUS *status, ChessType* type[2]) = 0;

  /* inputs an array of ChessTypes of both players in type[4][2],
   * returns black's score in score[0], white's in score[1] */
  static virtual evaluateScore(ChessType* type[4][2], int *score) = 0;


  static evaluateRelativeScore(VirtualBoardGomoku::Point* point[15][15], int playNo);

 protected:
  class OpeningTree;

 private:
  static const int SCORE_WIN = 10000000;
  static bool isInitialized;

};

#endif