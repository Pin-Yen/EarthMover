#include <iostream>
#include <string>
#include <ctype.h>
#include "chessboard.hpp"

using namespace std;

void play(ChessBoard* board);

int getColumn(int boardDimen);

int getRow(int boardDimen);

int main()
{
	ChessBoard* board = new ChessBoard();
	board->invalidate();

  play(board);
}

void play(ChessBoard* board)
{
  while (true)
  {
    int row, col;
    
    STATUS status;

    while (true)
    {
      cout << (board->isBlackTurn() ? "Black turn\n" : "White turn\n");

      col = getColumn(board->CHESSBOARD_DIMEN);

      row = getRow(board->CHESSBOARD_DIMEN);

      status = board->isBlackTurn() ? STATUS::BLACK : STATUS::WHITE;

      if (board->play(status, row, col)) break;

      cout << "Invalid move\n";
    }
    
    cout << endl;

    status = board->judge();

    if (status != STATUS::EMPTY)
    {
      cout << (status == STATUS::BLACK ? "Black" : "White") << "win !\n\n";

      board->wipe(true);
    }
  }
}

int getColumn(int boardDimen)
{
  int col;

  while (true)
  {
    cout << "enter the colume of next move (A~" << (char)('A' + boardDimen - 1) << ") : ";
    string input;
    cin >> input;

    if (input.length() == 1)
    {
      col = input[0];
      if (col >= 'A' && col <= 'A' + boardDimen)
      {
        col -= 'A'; break;
      }
      else if (col >= 'a' && col < 'a' + boardDimen)
      {
        col -= 'a'; break;
      }
    }

    cout << "Invalid colume\n";
  }

  return col;
}

int getRow(int boardDimen)
{
  int row;

  while (true)
  {
    cout << "enter the row of next move (1~" << boardDimen << ") : ";
    string input;
    cin >> input;

    bool isNumber = true;
    for (int i = 0, n = input.length(); i < n; i++)
      if (!isdigit(input[i]))
      {
        isNumber = false; break;
      }

      if (isNumber)
      {
        row = stoi(input);
        if (row >= 1 && row <= boardDimen)
        {
          row--; break;
        }
      }
      
      cin.clear();
      cout << "Invalid row\n";
  }

  return row;
}
