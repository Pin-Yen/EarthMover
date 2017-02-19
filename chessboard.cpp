#include "chessboard.hpp"
#include <stdio.h>
#include <assert.h>

#define CHESSBOARD_DIMEN 19
#define STATUS_EMPTY 0
#define STATUS_BLACK 1
#define STATUS_WHITE 2
#define CHESS_BLACK "○"
#define CHESS_WHITE "●"

ChessBoard::ChessBoard(){

	/* initialize pointStatus */
	for(int i=0;i<CHESSBOARD_DIMEN;i++)
		for(int k=0;k<CHESSBOARD_DIMEN;k++)
			pointStatus[i][k] = 0;

}

/* prints the current chesssboard */
ChessBoard::invalidate(){
	
	for(int r=0;r<CHESSBOARD_DIMEN;r++){
		for(int c=0;c<CHESSBOARD_DIMEN;c++){
			switch(pointStatus[r][c]){
				case STATUS_EMPTY:
					printf(" ");break;
				case STATUS_BLACK:
					printf(CHESS_BLACK);break;
				case STATUS_WHITE:
					printf(CHESS_WHITE);break;
			}
		}
	}

}

/* puts a new chess */
void ChessBoard::play(byte color,int row,int col){
	if(pointStatus[row][col] != STATUS_EMPTY)
		assert(0);
	else
		pointStatus[row][col] = color;
	invalidate();

}

/* clears the whole game */
void ChessBoard::wipe(){
	for(int i=0;i<CHESSBOARD_DIMEN;i++)
		for(int k=0;k<CHESSBOARD_DIMEN;k++)
			pointStatus[i][k] = STATUS_EMPTY;
	invalidate();
}
