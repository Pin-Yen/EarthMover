#ifndef POINT_STATUS_H
#define POINT_STATUS_H
#include "chesstype.hpp"
#include "typetree.hpp"
#define LENGTH 9

class PointStatus{
	
public:

	PointStatus(int r,int c);

	/* direction index 0↔ 1↕ 2⤢ 3⤡
	 * update neighbors (called by Evaluator::notifyNewMove */
	void updatePoint(int dir, int changedR, int changedC, int color);

private:
	int r,c;
	ChessType* directionType[4];
	Status dirStatus[4][LENGTH];
	const bool checkForbidden = false;
		
	/* give this point a score according to the types of four directions. */
	updateScore();
}

#endif