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
    
    bool blackTurn = board->isBlackTurn();

    /* get user's input and try to play, if the input is not valid,*/
    /* it will keep ask another input*/
    while (true)
    {
      std::cout << (blackTurn ? "Black turn\n" : "White turn\n");

      /* get user input*/
      board->getInput(row, col);

      /* break while user input a valid coordinate*/
      if (board->play(row, col)) 
      {
        vBoard->play(row, col);
        break;
      }

      std::cout << "Invalid move\n";
    }

    if (vBoard->getScore(row, col, !blackTurn) == 1000000)
    {
      std::cout << (blackTurn ? "Black" : "White") << " win !\n\n";

      board->wipe();

      vBoard = new VirtualBoard();
    }

    int r, c;
    vBoard->getHSP(r, c, blackTurn);
    std::cout << "highest position: " << (char)(c + 65) << r + 1 << 
      " score: " << vBoard->getScore(r, c, blackTurn) << std::endl;
  }
}