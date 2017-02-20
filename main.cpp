#include <stdio.h>
#include "chessboard.hpp"

int main()
{
	ChessBoard chessBoard;
	chessBoard.invalidate();

  chessBoard.play(STATUS::BLACK, 3, 6);
}