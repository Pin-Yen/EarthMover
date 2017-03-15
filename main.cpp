#include <iostream>
#include <string>
#include "gomoku/displayboard.hpp"
#include "gomoku/freestyle/virtualboard.hpp"

void start(DisplayBoard* board, VirtualBoard* vBoard);

int main()
{
  TypeTree::initialize();

  DisplayBoard* displayBoard = new DisplayBoard();
  VirtualBoard* virtualBoard = new VirtualBoard();

  start(displayBoard, virtualBoard);
}

void start(DisplayBoard* board, VirtualBoard* vBoard)
{
  while (true)
  {
    int row, col;
    
    /* get who turn, 0 = black, 1 = white*/
    bool whoTurn = board->getWhoTurn();

    /* debugger */
    int r, c; vBoard->getHSP(r, c);
    std::cout << "highest position: " << (char)(c + 65) << r + 1 << 
      " score: " << vBoard->getScore(r, c, whoTurn) << std::endl;

    /* get user's input and try to play, if the input is not valid,*/
    /* it will keep ask another input*/
    while (true)
    {
      std::cout << (whoTurn ? "White turn\n" : "Black turn\n");

      /* get user input*/
      board->getInput(row, col);

      /* break while user input a valid coordinate*/
      if (board->play(row, col)) break;

      std::cout << "Invalid move\n";
    }

    if (vBoard->play(row, col))
    {
      std::cout << (whoTurn ? "White" : "Black") << " win !\n\n";

      board->wipe();

      vBoard = new VirtualBoard();
    }
  }
}