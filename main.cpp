// mutual dependencies
#include "gametree.hpp"
#include "virtualboard.hpp"

#include "gomoku/chesstype.hpp"
#include "gomoku/displayboard.hpp"
#include "gomoku/point.hpp"
#include "gomoku/status.hpp"
#include "gomoku/virtualboardgomoku.hpp"
// freestyle dependencies
#include "gomoku/freestyle/virtualboardfreestyle.hpp"
// renju_basic dependencies
#include "gomoku/renju_basic/virtualboardrenjubasic.hpp"



#include <time.h>
#include <assert.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>

#ifdef DEBUG
#include "objectcounter.hpp"
#endif

#ifdef ANALYZE
#include "log.hpp"
#endif

void start();
void start_AI();

int main() {
  #ifndef TIME
  #ifndef DEBUG
  srand((unsigned)time(NULL));
  #endif
  #endif

  #ifdef DEBUG
  ObjectCounter::printInfo();
  #endif

  #ifdef ANALYZE
  Log::init();
  #endif

  start();

  #ifdef ANALYZE
  Log::closeLog();
  #endif

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


  #ifdef ANALYZE
  Log log;
  #endif

  while (true) {
    int row, col;

    bool whoTurn = board->whoTurn();

    #ifdef ANALYZE
    log << "==== PLAY #" << board->playNo() << " ====\n";
    #endif

    std::cout << "AI searching..." << std::endl;

    #ifdef TIME
    clock_t start, finish;
    start = clock();
    #endif

    tree->MCTS(cycle);
    tree->MCTSResult(&row, &col);

    #ifdef TIME
    finish = clock();
    std::cout << "duration: " << (double)(finish - start) / CLOCKS_PER_SEC << std::endl;
    #endif

    #ifdef DEBUG
    ObjectCounter::printInfo();
    #endif

    #ifndef ANALYZE
    bool stop = false;

    std::thread backgroundThread([tree](int maxCycle, bool &stop)
                                 { tree->MCTS(maxCycle, stop); },
                                 100000, std::ref(stop));
    #endif

    bool validInput = false;

    while (!validInput) {
      /* get user input*/
      board->getInput(&row, &col);

      /* tries to play at (row, col) */
      validInput = board->play(row, col);

      /* handle invalid input */
      if (!validInput)
        std::cout << "Invalid move\n";
    }

    #ifndef ANALYZE
    stop = true;
    backgroundThread.join();
    #endif

    /* update tree and handle result */
    int result = tree->play(row, col);
    switch (result) {
      case 1 :
        std::cout << "Five in a row !\n"
                  << (!whoTurn ? "black" : "white") << " wins\n";
        break;
      case -1 :
        std::cout << "Black played on a forbidden point !\nwhite wins\n";
    }
    if (result != 0) break;
  }
}