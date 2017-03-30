#include "gomoku/freestyle/status.hpp"
#include "gomoku/freestyle/chesstype.hpp"
#include "gomoku/freestyle/virtualboard.hpp"
#include "gomoku/displayboard.hpp"
#include "gametree.hpp"

#include <time.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <cstdlib>

#ifdef DEBUG
#include "objectcounter.hpp"
#include "log.hpp"
#endif

void start();
void start_AI();

int main() {
  srand((unsigned)time(NULL));

  #ifdef DEBUG
  ObjectCounter::printInfo();
  Log::init();
  #endif

  //start();
  start_AI();

  #ifdef DEBUG
  Log::closeLog();
  #endif

  return 0;
}

void start_AI() {
  DisplayBoard* board = new DisplayBoard();
  GameTree* tree = new GameTree();

  #ifdef DEBUG
  Log log;
  #endif

  while (true) {
    int row, col;

    bool whoTurn = board->getWhoTurn();

    #ifdef DEBUG
    log << "==== PLAY #" << board->getPlayNo() << " ====\n";
    #endif

    std::cout << "AI searching..." << std::endl;
    tree->MCTS(row, col, 1000);

    #ifdef DEBUG
    ObjectCounter::printInfo();
    #endif

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
  GameTree* tree = new GameTree();

  while (true) {
    int row, col;

    /* get who turn, 0 = black, 1 = white*/
    bool whoTurn = board->getWhoTurn();

    // debugger
    VirtualBoard* virtualBoard = tree->getCurrentBoard();
    virtualBoard->getHSP(row, col);
    std::cout << "highest position: "
              << (char)(col + 65) << row + 1
              << " score: "
              << virtualBoard->getScore(row, col, whoTurn) << std::endl;
    // end debugger

    /* get user's input and try to play, if the input is not valid,*/
    /* it will keep ask another input*/
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

    // debugger
    std::cout << (char)(col + 65) << row + 1
              << " score: "
              << virtualBoard->getScore(row, col, whoTurn) << std::endl;
    // end debugger

    /* update tree and handle result */
    if (tree->play(row, col)) {
      /* somebody wins */
      bool winner = board->getWhoTurn();
      std::cout << (winner ? "black" : "white") << " wins\n";
      break;
    }
  }
}