#include "typetree.hpp"
#include "chesstype.hpp"
#include "status.hpp"

class Evaluator
{
public:
  static void evaluate_type(ChessType* type[2], STATUS *status);  

  static void evaluate_score(ChessType* type[4][2], int *score);
};
