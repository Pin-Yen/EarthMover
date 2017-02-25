#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "pointstatus.hpp"
#include "typetree.hpp"

class Evaluator
{
public:

	Evaluator(){

	}
	~Evaluator();

	/* notifies the neighbors of (r,c) ot update their neighbors*/
	void notifyNewMove(int r, int c, int color);

private:
	PointStatus *pointstatus[ChessBoard::CHESSBOARD_DIMEN][ChessBoard::CHESSBOARD_DIMEN];
	
};
#endif