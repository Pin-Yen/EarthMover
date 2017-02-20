#include <iostream>
#include <string>

#include "chessboard.hpp"

using namespace std;

int main()
{
	ChessBoard chessBoard;
	chessBoard.invalidate();

  while (true)
  {
    int row, col;

    cout << (chessBoard.isBlackTurn() ? "Black turn" : "White turn");
    
    while (true)
    {
      cout << "\nenter the colume of next move (A~O) :";
      string input;
      cin >> input;

      if (input.length() == 1)
      {
        col = input[0];
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
      }

      cout << "Invalid colume\n";
    }

    while (true)
    {
      cout << "enter the row of next move (1~15) :";
      string input;
      cin >> input;

      bool isNumber = true;
      for (int i = 0, n = input.length(); i < n; i++)
      {
        if (!(input[i] >= 48 && input[i] <= 57))
        {
          isNumber = false;
          break;
        }

      }
      if (isNumber)
      {
        row = stoi(input);
        if (row >= 1 && row <= 15)
        {
          row--;
          break;
        }
      }
      
      cin.clear();
      cout << "Invalid row\n";
    }
    
    STATUS status = chessBoard.isBlackTurn() ? STATUS::BLACK : STATUS::WHITE;

    chessBoard.play(status, row, col);

    cout << endl;
  }
}