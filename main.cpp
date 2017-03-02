#include <iostream>
#include <string>
#include <ctype.h>
#include "displayboard.hpp"

void start(DisplayBoard* board);

void getInput(int* row, int* col, int boardDimen);

int main()
{
  DisplayBoard* board = new DisplayBoard();

  start(board);
}

void start(DisplayBoard* board)
{
  while (true)
  {
    int row, col;
    
    bool blackTurn = board->isBlackTurn();

    /* get user's input and try to play, if the input is not valid,*/
    /* it will keep ask another input*/
    while (true)
    {
      std::cout << (blackTurn ? "Black turn\n" : "White turn\n");

      /* get user input*/
      getInput(&row, &col, board->CHESSBOARD_DIMEN);

      /* break while user input a valid coordinate*/
      if (board->play(row, col)) break;

      std::cout << "Invalid move\n";
    }

    if (board->judge(row, col))
    {
      std::cout << (blackTurn ? "Black" : "White") << " win !\n\n";

      board->wipe();
    }
  }
}

/* get user input, it will keep ask another input until user coorperation*/
void getInput(int* row, int* col, int boardDimen)
{
  int r, c;
  while (true)
  {
    std::cout << "enter the coordinate of next move (A1 ~ " 
      << (char)('A' + boardDimen - 1) << boardDimen << ") : ";
    std::string input;
    std::cin >> input;

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
      for (int i = 1; i < n; ++i)
        if (!isdigit(input[i]))
        {
          isNumber = false; break;
        }

      if (isNumber)
      {
        r = std::stoi(input.substr(1, n - 1));
        if (r >= 1 && r <= boardDimen)
        {
          r--;
          validRow = true;
        }
      }   

      if (validRow && validColumn) break;  
    }

    std::cout << "Invalid input\n";
  }

  *row = r; *col = c;
}