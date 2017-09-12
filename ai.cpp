#include "ai.h"

#include <assert.h>
#include <iostream>

#include "gomoku/freestyle/virtualboardfreestyle.h"
#include "gomoku/renju_basic/virtualboardrenjubasic.h"
#include "const.h"

AI::AI() {
  tree = new GameTree();
  backgroundThread = NULL;
}

AI::~AI() {
  if (tree != NULL) delete tree;
}

int AI::think() {
  switch (level_) {
    case 0:
      tree->mcts(3000, 1500); break;
    case 1:
      tree->mcts(3000, 3000); break;
    case 2:
      tree->mcts(4, 3000, 3000); break;
    default:
      assert(false);
  }

  return tree->mctsResult();
}

int AI::play(int index) {
  int winning = tree->play(index);
  // winning  whoTurn   winner
  //    0                 -1
  //    0                 -1
  //   -1        0         1
  //   -1        1         0
  //    1        0         0
  //    1        1         1

  return winning == 0 ? -1 :
      ((winning == -1) ^ tree->getCurrentBoard()->whoTurn());
}

void AI::reset(int level, int rule) {
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

void AI::thinkInBackground(bool* continueThinking) {
  tree->mcts(MAX_BACKGROUND_CYCLE_, continueThinking);
}
