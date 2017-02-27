#include "chessboard.hpp"

int evaluate(ChessType* type[4][2], STATUS *status, int dir, bool checkForbidden)
{

  type[dir] = TypeTree::classify(status, checkForbidden);


  int score;

  //TODO: score this point according the type[4][2]



  return score;
}
