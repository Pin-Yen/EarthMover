#include "ai.hpp"
#include <assert.h>


AI::AI(int cycle, DisplayBoard *board) {
  this->cycle = cycle;
  this->board = board;
  tree = new GameTree();
  backgroundThread = NULL;
}

AI::~AI() {
  if (board != NULL)
    delete board;
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

  tree->MCTS(cycle);
  tree->MCTSResult(*row, *col);

}

bool AI::play(int row, int col, bool triggerBackgroundThread ) {
  bool hasSomeoneWin = tree->play(row, col);

  if (triggerBackgroundThread) {
    stopBackgroundThread = false;
    GameTree* treeRef = tree;

    backgroundThread = new std::thread([treeRef](int maxCycle, bool &stop)
                                        { treeRef->MCTS(maxCycle, stop); },
                                        100000, std::ref(stopBackgroundThread));
  }

  return hasSomeoneWin;
}

void AI::stopBGThread() {
  if (backgroundThread != NULL) {
    stopBackgroundThread = true;
    backgroundThread->join();
    delete backgroundThread;
    backgroundThread = NULL;
  }
}