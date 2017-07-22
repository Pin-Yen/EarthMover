#include "ai.hpp"
#include "gomoku/freestyle/virtualboardfreestyle.hpp"
#include "gomoku/renju_basic/virtualboardrenjubasic.hpp"
#include "const.hpp"

#include <assert.h>
#include <iostream>

AI::AI() {
  tree = new GameTree();
  backgroundThread = NULL;
}

AI::~AI() {
  if (tree != NULL) delete tree;
}

int AI::think() {
  stopBGThread();

  switch (level_) {
    case 0:
      tree->MCTS(2000); break;
    case 1:
      tree->MCTS(2000, 1000); break;
    case 2:
      tree->MCTS(2000, 2000);
  }

  return tree->MCTSResult();
}

int AI::play(int index, bool triggerBackgroundThread) {
  // stop background thinking to avoid memory corruption.
  stopBGThread();
  int result = tree->play(index);

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

  VirtualBoard* vb;

  switch (rule) {
    case GOMOKU_FREESTYLE: vb = new VirtualBoardFreeStyle(); break;
    case GOMOKU_RENJU_BASIC: vb = new VirtualBoardRenjuBasic(); break;
    default: assert(0);
  }

  tree->reset(vb);
  level_ = level;

  delete vb;
}