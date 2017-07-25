#include "ai.hpp"
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

int AI::think() {

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

int AI::play(int index) {
  int winning = tree->play(index);
   /* winning  whoTurn   winner
   *    0                 -1
   *    0                 -1
   *   -1        0         1
   *   -1        1         0
   *    1        0         0
   *    1        1         1 */

  return winning == 0 ? -1 : ((winning == -1) ^ tree->getCurrentBoard()->whoTurn());
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

void AI::thinkInBackground(bool* continueThinking) {
  MCTS(MAX_BACKGROUND_CYCLE_, continueThinking);
}