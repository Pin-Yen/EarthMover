#include "ai.hpp"
// #include "virtualboard.hpp"
// #include "gomoku/virtualboardgomoku.hpp"
#include "gomoku/freestyle/virtualboardfreestyle.hpp"
#include "gomoku/renju_basic/virtualboardrenjubasic.hpp"
#include "const.hpp"

#include <assert.h>
#include <iostream>

AI::AI() {
  tree = new GameTree();
  backgroundThread = NULL;
  vb = NULL;
}

AI::~AI() {
  if (tree != NULL) delete tree;
  if (vb != NULL) delete vb;
}

void AI::think(int *row, int *col) {
  stopBGThread();

  #ifdef DEBUG_MCTS_PROCESS
    int batch = 100;
    int batchSize = cycle / batch;
    int cycles = cycle / batch;

    for (int c = 0; c < cycles; ++c) {
      tree->MCTS(batchSize);
      std::cout << "batch " << c << ":\n";
      tree->MCTSResult(*row, *col);
    }
    printf("============================================\n");
  #else
    switch (level_) {
      case 0:
        tree->MCTS(2000); break;
      case 1:
        tree->MCTS(2000, 1000); break;
      case 2:
        tree->MCTS(2000, 2000);
    }

    tree->MCTSResult(*row, *col);
  #endif
}

int AI::play(int row, int col, bool triggerBackgroundThread ) {
  // stop background thinking to avoid memory corruption.
  stopBGThread();
  int result = tree->play(row, col);

  if (triggerBackgroundThread)
    startBGThread();

  return result;
}

void AI::startBGThread() {
  stopBackgroundThread = false;
  GameTree* treeRef = tree;

  backgroundThread = new std::thread([treeRef](int maxCycle, bool &stop)
                                     { treeRef->MCTS(maxCycle, stop); },
                                     100000, std::ref(stopBackgroundThread));
}

void AI::stopBGThread() {
  if (backgroundThread != NULL) {
    stopBackgroundThread = true;
    backgroundThread->join();
    delete backgroundThread;
    backgroundThread = NULL;
  }
}

void AI::reset(int level, int rule) {
  stopBGThread();

  if (vb != NULL)
    delete vb;

  switch (rule) {
    case GOMOKU_FREESTYLE: vb = new VirtualBoardFreeStyle(); break;
    case GOMOKU_RENJU_BASIC: vb = new VirtualBoardRenjuBasic(); break;
    default: assert(0);
  }

  tree->reset(vb);
  level_ = level;
}

bool AI::whoTurn() {
  return tree->getCurrentBoard()->whoTurn();
}