#include "ai.hpp"
#include <assert.h>
#include <iostream>

AI::AI() {
  tree = new GameTree();
  backgroundThread = NULL;
}

AI::~AI() {
  if (tree != NULL)
    delete tree;
}

void AI::think(int *row, int *col) {
  if (backgroundThread != NULL) {
    stopBackgroundThread = true;
    backgroundThread->join();
    delete backgroundThread;
    backgroundThread = NULL;
  }

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

bool AI::play(int row, int col, bool triggerBackgroundThread ) {
  // stop background thinking to avoid memory corruption.
  stopBGThread();
  bool hasSomeoneWin = tree->play(row, col);

  if (triggerBackgroundThread) {
    startBGThread();
  }

  return hasSomeoneWin;
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

void AI::reset(int level) {
  stopBGThread();
  tree->reset();
  level_ = level;
}

bool AI::whoTurn() {
  return tree->getCurrentBoard()->whoTurn();
}