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

void AI::think(int clientRow, int clientCol, int *row, int *col) {
  // TODO, check if the user wins (or, an alternative, let the board handle it)
  if( clientCol != -1)
    int result = tree->play(*row, *col);

  if (backgroundThread != NULL) {
    stopBackgroundThread = true;
    backgroundThread->join();
    delete backgroundThread;
  }

  tree->MCTS(cycle);
  tree->MCTSResult(*row, *col);

}

bool AI::play(int row, int col) {
  bool hasSomeoneWin = tree->play(row, col);

  stopBackgroundThread = false;
  GameTree* treeRef = tree;

  backgroundThread = new std::thread([treeRef](int maxCycle, bool &stop)
                              { treeRef->MCTS(maxCycle, stop); },
                              100000, std::ref(stopBackgroundThread));
  return hasSomeoneWin;
}