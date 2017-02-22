#include <iostream>
#include <string>
#include <ctype.h>
#include "chessboard.hpp"

using namespace std;

void play(ChessBoard* board);

void getInput(int* row, int* col, int boardDimen);

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
    
    STATUS status = board->isBlackTurn() ? STATUS::BLACK : STATUS::WHITE;

    /* get user's input and try to play, if the input is not valid,*/
    /* it will keep ask another input*/
    while (true)
    {
      cout << (status == STATUS::BLACK ? "Black turn\n" : "White turn\n");

      /* get user input*/
      getInput(&row, &col, board->CHESSBOARD_DIMEN);

      /* break while user input a valid coordinate*/
      if (board->play(status, row, col)) break;

      cout << "Invalid move\n";
    }
    
    cout << endl;

    if (board->judge(status, row, col))
    {
      cout << (status == STATUS::BLACK ? "Black" : "White") << " win !\n\n";

      board->wipe(true);
    }
  }
}

/* get user input, it will keep ask another input until user coorperation*/
void getInput(int* row, int* col, int boardDimen)
{
  int r, c;
  while (true)
  {
    cout << "enter the coordinate of next move (A1 ~ " 
      << (char)('A' + boardDimen - 1) << boardDimen << ") : ";
    string input;
    cin >> input;

    int n = input.length();

    if (n == 2 || n == 3)
    {
      bool validColumn = false, validRow = false;

      /* get column*/
      c = input[0];
      if (c >= 'A' && c <= 'A' + boardDimen)
      {
        c -= 'A';
        validColumn = true;
      }
      else if (c >= 'a' && c <= 'a' + boardDimen)
      {
        c -= 'a';
        validColumn = true;
      }

      /* get row*/
      bool isNumber = true;
      for (int i = 1; i < n; i++)
        if (!isdigit(input[i]))
        {
          isNumber = false; break;
        }

      if (isNumber)
      {
        r = stoi(input.substr(1, n - 1));
        if (r >= 1 && r <= boardDimen)
        {
          r--;
          validRow = true;
        }
      }   

      if (validRow && validColumn) break;  
    }

    cout << "Invalid input\n";
  }

  *row = r; *col = c;
}
