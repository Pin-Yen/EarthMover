#include "gomoku/freestyle/status.hpp"
#include "gomoku/freestyle/chesstype.hpp"
#include "gomoku/freestyle/typetree.hpp"
#include "gomoku/freestyle/virtualboard.hpp"
#include "gomoku/displayboard.hpp"
#include "gametree.hpp"

#include <iostream>
#include <string>
#include <assert.h>

void start(DisplayBoard* board, VirtualBoard* vBoard);
void start(DisplayBoard* board, bool computerColor);

int main() {
  TypeTree::initialize();

  DisplayBoard* displayBoard = new DisplayBoard();
  VirtualBoard* virtualBoard = new VirtualBoard();

  #ifdef TWO_PLAYER
    start(displayBoard, virtualBoard);
  #else
    start(displayBoard, true);
  #endif

  return 0;
}

void start(DisplayBoard *board, bool computerColor) {
  GameTree* tree = new GameTree();

  while (true) {
    int row, col;

    bool whoTurn = board->getWhoTurn();

    std::cout << "AI searching..." << std::endl;
    tree->MCTS(row, col, 1000);

    std::cout << "best point: " << (char)(col + 65) << row + 1 << std::endl;

    bool validInput = false;

    while (!validInput) {
      /* get user input*/
      board->getInput(row, col);

      /* tries to play at (row, col) */
      validInput = board->play(row, col);

      /* handle invalid input */
      if (!validInput)
        std::cout << "Invalid move\n";
    }

    /* update tree and handle result */
    if (tree->play(row, col)) {
      /* somebody wins */

      bool winner = board->getWhoTurn();
      std::cout << (winner ? "black" : "white") << " wins\n";
      break;
    }
  }
}

void start(DisplayBoard* board, VirtualBoard* vBoard) {
  while (true) {
    int row, col;

    /* get who turn, 0 = black, 1 = white*/
    bool whoTurn = board->getWhoTurn();

    /* debugger */
    int r, c; vBoard->getHSP(r, c);
    std::cout << "highest position: "
              << (char)(c + 65) << r + 1
              << " score: "
              << vBoard->getScore(r, c, whoTurn) << std::endl;

    /* get user's input and try to play, if the input is not valid,*/
    /* it will keep ask another input*/
    while (true) {
      std::cout << (whoTurn ? "White turn\n" : "Black turn\n");

      /* get user input*/
      board->getInput(row, col);

      /* break while user input a valid coordinate*/
      if (board->play(row, col)) break;

      std::cout << "Invalid move\n";
    }

    if (vBoard->play(row, col)) {
      std::cout << (whoTurn ? "White" : "Black") << " win !\n\n";

      board->wipe();

      vBoard = new VirtualBoard();
    }
  }
}