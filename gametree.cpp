#include "gametree.h"

#include <assert.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>
#include <tuple>
#include <vector>

#include "gomoku/chesstype.h"
#include "gomoku/status.h"
#include "lib/json.h"
#include "memorypool.h"
#include "node.h"
#include "virtualboard.h"

using json = nlohmann::json;

//MemoryPool GameTree::pool_;

GameTree::GameTree() {
  root_ = NULL;
  currentNode_ = NULL;
  currentBoard_ = NULL;
}

GameTree::~GameTree() {
  //pool_.free();
  if (currentBoard_ != NULL) delete currentBoard_;
  if (root_ != NULL) root_->deleteChildren();
}

void GameTree::reset(VirtualBoard* board) {
  const int MAX_NODE = 2000000;

  if (root_ != NULL) root_->deleteChildren();
  //pool_.free();
  //pool_.init(sizeof(Node), MAX_NODE);
  root_ = new Node();

  currentNode_ = root_;

  if (currentBoard_ != NULL)
    delete currentBoard_;
  currentBoard_ = board->create();
}

bool GameTree::mcts(int cycle) {
  Node* node;

  for (int i = 0; i < cycle; ++i) {
    if (!currentNode_->notWinOrLose()) return false;

    VirtualBoard* board = currentBoard_->clone();

    int result = selection(&node, board);

    // simulate only if child is not winning
    if (result == -2) {
      result = simulation(board);
    }

    if (result == -1)
      backProp(node);
    else
      backProp(node, result);

    delete board;
  }
  return true;
}

void GameTree::mcts(int batch, int minCount) {
  Node* node;

  while (true) {
    if (!mcts(batch)) return;

    // get mostTimes
    int mostTimes = 0;

    for (Node* child : *currentNode_) {
      if (child->totalPlayout() > mostTimes) {
        mostTimes = child->totalPlayout();
      }
    }

    // mostTimes == 0 means that no useful point(can pass now)
    if (mostTimes >= minCount || mostTimes == 0) return;
  }
}

void GameTree::mcts(int maxCycle, const bool* continueThinking) {
  int batchCycle = maxCycle / 1000;
  for (int i = 0; i < batchCycle && *continueThinking; ++i) {
    if (!mcts(1000)) return;
  }
}

void GameTree::mcts(const bool* continueThinking) {
  const int BATCH = 1000;
  while (*continueThinking) {
    if (!mcts(1000)) return;
  }
}

void GameTree::mcts(int threadAmount, int batch, int minCount) {
  assert(threadAmount > 1);
  std::thread** threads = new std::thread*[threadAmount - 1];

  GameTree** trees = new GameTree*[threadAmount - 1];
  for (int i = 0; i < threadAmount - 1; ++i) {
    trees[i] = copyTree();
  }
  bool continueThinking = true;
  bool* controler = &continueThinking;
  for (int i = 0; i < threadAmount - 1; ++i) {
    threads[i] = new std::thread([trees, i](bool* controler)
                                 { trees[i]->mcts(controler); },(controler));
  }
  mcts(batch, minCount);

  *controler = false;
  for (int i = 0; i < threadAmount - 1; ++i) {
    threads[i]->join();
    mergeTree(trees[i]);
    delete threads[i];
    delete trees[i];
  }
  delete [] threads;
  delete [] trees;
}

int GameTree::mctsResult() const {
  int index;

  // if is winning, select it
  if (currentNode_->winning()) {
    for (Node* child : *currentNode_) {
      if (child->losing()) index = child->index();
    }
  } else {
    // select child that has max playout
    int mostTimes = -1, score = 0;

    for (Node* child : *currentNode_) {
      // priority order: playout -> score
      if (child->totalPlayout() > mostTimes) {
        index = child->index();
        mostTimes = child->totalPlayout();
        score = currentBoard_->getScore(index);
      } else if (child->totalPlayout() == mostTimes) {
        if (currentBoard_->getScore(child->index()) > score) {
          index = child->index();
          mostTimes = child->totalPlayout();
          score = currentBoard_->getScore(index);
        }
      }
    }

    // pass
    if (mostTimes == -1) {
      std::cout << "pass" << std::endl;
      return -1;
    }
  }

  Node* child = currentNode_->child(index);

  std::cout << std::fixed << std::setprecision(3)
            << "total sim: " << currentNode_->totalPlayout()
            << "  best: "
            << static_cast<char>(index % 15 + 65) << index / 15 + 1
            << "  sim: " << child->totalPlayout()
            << "  WinR: " << child->winRate()
            << "  W/L: " << currentNode_->winOrLose()
            << std::endl;

  return index;
}

int GameTree::selection(Node** node, VirtualBoard* board) const {
  *node = currentNode_;

  while (true) {
    int index;

    int result = (*node)->selection(&index, board);
    if (result != -2) {
      return result;
    }

    // check if reached leaf
    Node* child = (*node)->child(index);

    if (child == NULL) {
      bool parentWinning = board->play(index);
      *node = (*node)->newChild(index, parentWinning);

      if (parentWinning)
        return 0;
      else
        return -2;
    }

    *node = child;

    board->play(index);
  }
}

int GameTree::simulation(VirtualBoard* board) const {
  const int MAX_DEPTH = 50;
  // simulate until reach max depth
  for (int d = 1; d <= MAX_DEPTH; ++d) {
    int index = board->getHSI();
    // return tie(-1) if no useful point
    if (index == -1) return -1;

    // if win, return
    if (board->play(index))
      return (d & 1);
  }

  return -1;
}

void GameTree::backProp(Node* node, bool result) {
  // note: cannot use do-while here
  while (node != currentNode_) {
    node->update(result);
    node = node->parent();
    result = !result;
  }
  node->update(result);
}

void GameTree::backProp(Node* node) {
  // note: cannot use do-while here
  while (node != currentNode_) {
    node->update();
    node = node->parent();
  }
  node->update();
}

void GameTree::copyAllChildren(Node* srcNode, Node* destNode) {
  for (Node* child : *srcNode) {
    Node* destChild = destNode->newChild(child);
    if (child->hasChild()) {
      copyAllChildren(child, destChild);
    }
  }
}

GameTree* GameTree::copyTree() {
  GameTree* tree = new GameTree();
  tree->root_ = new Node(currentNode_->parent(), currentNode_);
  tree->currentNode_ = tree->root_;
  tree->currentBoard_ = currentBoard_->clone();
  copyAllChildren(currentNode_, tree->root_);

  return tree;
}

void GameTree::mergeAllChildren(Node* mergingNode, Node* mergedNode) {
  for (Node* mergedChild : *mergedNode) {
    Node* mergingChild = mergingNode->child(mergedChild->index());
    if (mergingChild != NULL) {
      mergingChild->merge(mergedChild);
    } else {
      mergingChild = mergingNode->newChild(mergedChild);
    }

    if (mergedChild->hasChild()) {
      mergeAllChildren(mergingChild, mergedChild);
    }
  }
}

void GameTree::mergeTree(GameTree* tree) {
  currentNode_->merge(tree->root_);
  mergeAllChildren(currentNode_, tree->root_);
}

int GameTree::play(int index) {
  int whoWin = currentBoard_->play(index);

  Node* child = currentNode_->child(index);

  if (child == NULL)
    child = currentNode_->newChild(index, whoWin);

  // delete other children except the child that going to play
  currentNode_->deleteChildrenExcept(child);
  currentNode_->clear();

  currentNode_ = child;

  return whoWin;
}

void GameTree::pass() {
  // get the index of pass
  int index = currentBoard_->pass();

  // return for not allow to pass
  if (index == -1) return;

  Node* child = currentNode_->child(index);

  if (child == NULL)
    child = currentNode_->newChild(index, 0);

  // delete other children except the child that going to play
  currentNode_->deleteChildrenExcept(child);
  currentNode_->clear();

  currentNode_ = child;
}

void GameTree::undo() {
  currentBoard_->undo(currentNode_->index());
  currentNode_ = currentNode_->parent();
  // delete all children
  currentNode_->deleteChildren();
}

std::string GameTree::getTreeJSON() {
  if (currentNode_ == NULL || currentNode_->parent() == NULL) return "";

  return getSubTreeJSON(currentNode_, currentBoard_->whoTurn()).dump();
}

json GameTree::getSubTreeJSON(Node* node, bool whoTurn) {
  json tree;

  tree["index"] = node->index();

  tree["totalCount"] = node->totalPlayout();

  tree["winRate"] = whoTurn ? node->winRate() : 1 - node->winRate();
  tree["winOrLose"] = whoTurn ? node->winOrLose() : -node->winOrLose();

  tree["whoTurn"] = whoTurn;

  tree["children"] = json::array();

  for (Node* child : *node) {
    if (child->totalPlayout() >= 8) {
      tree["children"].push_back(getSubTreeJSON(child, !whoTurn));
    }
  }
  return tree;
}
