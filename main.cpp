#include <iostream>

#include "chessboard.hpp"

using namespace std;

int main()
{
	ChessBoard chessBoard;
	chessBoard.invalidate();

  while (true)
  {
    int row;
    char col;

    cout << (chessBoard.isBlackTurn() ? "Black turn" : "White turn");
    
    while (true)
    {
      cout << "\nenter the colume of next move (A~O) :";
      cin >> col;

      if (col >= 65 && col < 80)
      {
        col -= 65;
        break;
      }
      else if (col >= 97 && col < 112)
      {
        col -= 97;
        break;
      }

      cout << "Invalid colume\n";
    }

    while (true)
    {
      cout << "enter the row of next move (1~15) :";
      cin >> row;

      if (row >= 1 && row <= 15)
      {
        row--;
        break;
      }

      cout << "Invalid colume\n";
    }
    
    ChessBoard::STATUS status = chessBoard.isBlackTurn() ? ChessBoard::BLACK : ChessBoard::WHITE;

    chessBoard.play(status, row, col);

    cout << endl;
  }
}