#include "ai.h"

#include <assert.h>
#include <iostream>
#include <time.h>

#include "gomoku/freestyle/virtualboardfreestyle.h"
#include "gomoku/renju_basic/virtualboardrenjubasic.h"
#include "const.h"

AI::AI() {
  tree = new GameTree();
}

AI::~AI() {
  if (tree != NULL) delete tree;
}

int AI::think() {
  switch (level_) {
    case 0:
      tree->mcts(2, 3200, 1600); break;
    case 1:
      tree->mcts(3, 3200, 2400); break;
    case 2:
      tree->mcts(4, 3200, 3200); break;
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

  tree->init(vb);
  level_ = level;

  delete vb;
}

void AI::thinkInBackground(bool* continueThinking) {
  tree->mcts(4, MAX_BACKGROUND_CYCLE_, continueThinking);
}

void AI::renewLiveTime() {
  lastLiveTime_ = time(NULL);
}

bool AI::isAlive() {
  time_t currentTime = time(NULL);
  double elapsedTime = difftime(currentTime, lastLiveTime_);

  if (elapsedTime <= MAX_KEEP_ALIVE_SEC_)
    return true;
  else 
    return false;
}