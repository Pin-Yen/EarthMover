#include <stdio.h>
#include "chessboard.hpp"

int main()
{
	ChessBoard chessBoard;
	chessBoard.invalidate();

  chessBoard.play(ChessBoard::BLACK, 3, 6);
}