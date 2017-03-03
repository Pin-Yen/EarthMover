#include "typetree.hpp"
#include "chesstype.hpp"
#include "status.hpp"
class evaluator{
public:
	static int* evaluate(ChessType* type[4][2], STATUS *status, int dir, bool checkForbidden);
};
