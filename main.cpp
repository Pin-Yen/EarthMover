#include <assert.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>

#include "gametree.h"
#include "virtualboard.h"

#include "gomoku/chesstype.h"
#include "gomoku/displayboard.h"
#include "gomoku/point.h"
#include "gomoku/status.h"
#include "gomoku/virtualboardgomoku.h"
#include "gomoku/freestyle/virtualboardfreestyle.h"
#include "gomoku/renju_basic/virtualboardrenjubasic.h"
#include "timer.h"

#ifdef DEBUG
#include "objectcounter.h"
#endif

void start();

int main() {
  #ifndef TIME
  #ifndef DEBUG
  srand((unsigned)time(NULL));
  #endif
  #endif

  #ifdef DEBUG
  ObjectCounter::printInfo();
  #endif

  start();

  return 0;
}

void start() {
  int cycle;
  std::cout << "simulate cycle: ";
  std::cin >> cycle;

  DisplayBoard* board = new DisplayBoard();
  GameTree* tree = new GameTree();

  int rule;
  std::cout << "freestyle (1), renju-basic (2)\n";
  std::cin >> rule;

  VirtualBoard *vb;
  switch (rule) {
    case 1: vb = new VirtualBoardFreeStyle(); break;
    case 2: vb = new VirtualBoardRenjuBasic(); break;
  }
  tree->reset(vb);


  while (true) {
    int row, col;

    bool whoTurn = board->whoTurn();

    std::cout << "AI searching..." << std::endl;

    #ifdef TIME
    Timer timer;
    timer.start();
    #endif

    tree->MCTS(cycle);
    tree->MCTSResult();

    #ifdef TIME
    timer.stop();
    timer.print();
    #endif

    #ifdef DEBUG
    ObjectCounter::printInfo();
    #endif

    #ifndef ANALYZE
    bool continueThinking = true;
    bool* controler = &continueThinking;

    std::thread backgroundThread([tree](int maxCycle, bool* controler)
                                 { tree->MCTS(maxCycle, controler); },
                                 100000, controler);
    #endif

    bool validInput = false;

    while (!validInput) {
      // get user input
      board->getInput(&row, &col);

      // tries to play at (row, col)
      validInput = board->play(row, col);

      // handle invalid input
      if (!validInput)
        std::cout << "Invalid move\n";
    }

    #ifndef ANALYZE
    continueThinking = false;
    backgroundThread.join();
    #endif

    // update tree and handle result
    int winning = tree->play(row * 15 + col);
    switch (winning) {
      case 1 :
        std::cout << "Five in a row !\n"
                  << (!whoTurn ? "black" : "white") << " wins\n";
        break;
      case -1 :
        std::cout << "Black played on a forbidden point !\nwhite wins\n";
    }
    if (winning != 0) break;
  }
}
