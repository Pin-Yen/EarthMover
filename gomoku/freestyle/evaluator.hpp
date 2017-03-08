#include "typetree.hpp"
#include "chesstype.hpp"
#include "status.hpp"

class Evaluator
{
public:
  static void evaluate(ChessType* type[4][2], STATUS *status, int dir, int *score);
};
