#include "chessboard.hpp"

Evaluator::Evaluator(){
	for(int r=0; r<ChessBoard::CHESSBOARD_DIMEN; r++)
		for(int c=0; c<ChessBoard::CHESSBOARD_DIMEN; c++)
			pointstatus[r][c] = (PointStatus*)new PointStatus(r,c);
}

Evaluator::~Evaluator(){
	for(int r=0; r<ChessBoard::CHESSBOARD_DIMEN; r++)
		for(int c=0; c<ChessBoard::CHESSBOARD_DIMEN; c++)
			delete pointstatus[r][c];
}

/* notifies the neighbors of (r,c) ot update their neighbors*/
void notifyNewMove(int r, int c, int color){
	const int direction[4][2] = {{1,0}, {0,1}, {-1,1}, {1,1}};
	for(int dir=0; dir<4; dir++){
		for(int offset = -4;offset <= 4; offset++){
			pointstatus[r + offset*direction[dir][0] ][c + offset*direction[dir][1] ]
			->updatePoint(dir, r, c, color);
		}			
	}
}
