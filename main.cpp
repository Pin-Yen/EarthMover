#include "gomoku/freestyle/status.hpp"
#include "gomoku/freestyle/chesstype.hpp"
#include "gomoku/freestyle/typetree.hpp"
#include "gomoku/freestyle/virtualboard.hpp"
#include "gomoku/displayboard.hpp"
#include "gametree.hpp"
#include "objectcounter.hpp"
#include "log.hpp"

#include <iostream>
#include <string>
#include <assert.h>

void start();
void start_AI();

int main() {
  TypeTree::initialize();

  ObjectCounter::printInfo();

  //start();
  Log::init();
  start_AI();
  Log::closeLog();
  return 0;
}

void start_AI() {
  DisplayBoard* board = new DisplayBoard();
  GameTree* tree = new GameTree();
  Log log;

  while (true) {
    int row, col;

    bool whoTurn = board->getWhoTurn();

    log << "==== PLAY #" << board->getPlayNo() << " ====\n";

    std::cout << "AI searching..." << std::endl;
    tree->MCTS(row, col, 1000);
    ObjectCounter::printInfo();

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

void start() {
  DisplayBoard* board = new DisplayBoard();
  VirtualBoard* vBoard = new VirtualBoard();

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