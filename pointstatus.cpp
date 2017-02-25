#include "pointstatus.hpp"

PointStatus::PointStatus(int r,int c){
	this->r = r;
	this->c = c;
}

/* direction index 0↔ 1↕ 2⤢ 3⤡
 * update neighbors (called by Evaluator::notifyNewMove */
void PointStatus::updatePoint(int dir, int changedR, int changedC, int color){
	switch (dir){
			case 0: /* ↔ */
		Status[0][changedC-c+ LENGTH/2] = color; break;
			case 1: /* ↕ */				
		Status[1][changedR-r+ LENGTH/2] = color; break;
			case 2: /* ⤢ */
		Status[2][changedC-c+ LENGTH/2] = color; break;
			case 3:/* ⤡ */
		Status[3][changedC-c+ LENGTH/2] = color; break;
	}

	//ERROR: typeAnalyze is not a static method !!!
	TypeTree::typeAnalyze(dirStatus[dir],checkForbidden);
}


PointStatus::updateScore(){
		//TODO:
		//give this point a score according to the types of four directions.
}
