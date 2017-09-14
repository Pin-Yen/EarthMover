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

GameTree::GameTree() : root_(NULL), currentNode_(NULL), currentBoard_(NULL) {}

GameTree::~GameTree() {
  pool_.free();
  if (currentBoard_ != NULL) delete currentBoard_;
}

void GameTree::init(VirtualBoard* board) {
  const int MAX_NODE = 800000;

  pool_.free();
  pool_.init(sizeof(Node), MAX_NODE);
  root_ = new(&pool_) Node();

  currentNode_ = root_;

  if (currentBoard_ != NULL)
    delete currentBoard_;
  currentBoard_ = board->create();
}

bool GameTree::mcts(int cycle) {
  for (int i = 0; i < cycle; ++i) {
    if (!currentNode_->notWinOrLose()) return false;

    VirtualBoard* board = currentBoard_->clone();

    std::pair<SearchStatus, Node*> result = selection(board);
    SearchStatus status = result.first;
    Node* node = result.second;

    // simulate only if child is not winning
    if (status == LEAF) {
      status = simulation(board);
    }

    backProp(node, status);

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

void GameTree::mcts(int maxCycle, const bool* thinking) {
  int batchCycle = maxCycle / 1000;
  for (int i = 0; i < batchCycle && *thinking; ++i) {
    if (!mcts(1000)) return;
  }
}

void GameTree::mcts(const bool* thinking) {
  while (*thinking) {
    if (!mcts(1000)) return;
  }
}

void GameTree::mcts(int threadAmount, int batch, int minCount) {
  // Maek sure more then one thread.
  assert(threadAmount > 1);

  int extraThreadAmount = threadAmount - 1;
  // Create threads and trees.
  std::thread** threads = new std::thread*[extraThreadAmount];
  GameTree** trees = new GameTree*[extraThreadAmount];
  for (int i = 0; i < extraThreadAmount; ++i) {
    trees[i] = new GameTree();
    trees[i]->copy(this);
  }

  // Copy origin tree.
  GameTree* originTree = new GameTree();
  originTree->copy(this);

  // Extra thread thinking.
  bool thinking = true;
  bool* controler = &thinking;
  for (int i = 0; i < extraThreadAmount; ++i) {
    threads[i] = new std::thread([trees, i, controler]
                                 { trees[i]->mcts(controler); });
  }
  // Origin thread thinking.
  mcts(batch, minCount);
  // Stop extra thread.
  thinking = false;

  // Join and delete thread.
  for (int i = 0; i < extraThreadAmount; ++i) {
    threads[i]->join();
    delete threads[i];
  }
  delete [] threads;

  for (int i = 0; i < extraThreadAmount; ++i) {
    minusTree(trees[i], originTree);
    mergeTree(trees[i]);
    delete trees[i];
  }
  delete [] trees;
  delete originTree;
}

void GameTree::mcts(int threadAmount, int maxCycle, const bool* thinking) {
  // Maek sure more then one thread.
  assert(threadAmount > 1);

  int extraThreadAmount = threadAmount - 1;
  // Create threads and trees.
  std::thread** threads = new std::thread*[extraThreadAmount];
  GameTree** trees = new GameTree*[extraThreadAmount];

  for (int i = 0; i < extraThreadAmount; ++i) {
    trees[i] = new GameTree();
    trees[i]->copy(this);
  }

  // Copy origin tree.
  GameTree* originTree = new GameTree();
  originTree->copy(this);

  // Extra thread thinking.
  for (int i = 0; i < extraThreadAmount; ++i) {
    threads[i] = new std::thread([trees, i, maxCycle, thinking]
                                 { trees[i]->mcts(maxCycle, thinking); });
  }

  // Origin thread thinking.
  mcts(maxCycle, thinking);

  // Join and delete thread.
  for (int i = 0; i < extraThreadAmount; ++i) {
    threads[i]->join();
    delete threads[i];
  }
  delete [] threads;
  for (int i = 0; i < extraThreadAmount; ++i) {
    minusTree(trees[i], originTree);
    mergeTree(trees[i]);
    delete trees[i];
  }
  delete [] trees;
  delete originTree;
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

std::pair<GameTree::SearchStatus, GameTree::Node*> GameTree::selection(
    VirtualBoard* board) {
  Node* node = currentNode_;

  while (true) {
    //int index;


    std::pair<SearchStatus, Node*> result = node->selection(board, &pool_);

    if (result.first != UNKNOWN) return result;

    node = result.second;
    // check if reached leaf
    //Node* child = (*node)->child(index);
    /*
    if (child == NULL) {
      bool parentWinning = board->play(index);
      *node = (*node)->newChild(index, parentWinning, &pool_);

      if (parentWinning)
        return WIN;
      else
        return UNKNOWN;
    }*/

    //*node = child;

    //board->play(index);
  }
}

GameTree::SearchStatus GameTree::simulation(VirtualBoard* board) const {
  const int MAX_DEPTH = 50;
  // simulate until reach max depth
  for (int d = 0; d < MAX_DEPTH; ++d) {
    int index = board->getHSI();
    // return tie if no useful point
    if (index == -1) return TIE;

    // if win, return
    if (board->play(index)) {
      return (d & 1) ? WIN : LOSE;
    }
  }

  return TIE;
}

void GameTree::backProp(Node* node, SearchStatus result) {
  while (node != currentNode_) {
    node->update(result);
    node = node->parent();
    // reverse result (WIN <-> LOSE)
    result = static_cast<SearchStatus>(-static_cast<int>(result));
  }
  node->update(result);
}

void GameTree::copy(const GameTree* source) {
  const int MAX_NODE = 320000;
  pool_.init(sizeof(Node), MAX_NODE);

  root_ = new(&pool_) Node(source->currentNode_->parent(), source->currentNode_);
  currentNode_ = root_;
  currentBoard_ = source->currentBoard_->clone();
  copyAllChildren(source->currentNode_, currentNode_);
}

void GameTree::copyAllChildren(const Node* srcNode, Node* destNode) {
  for (Node* child : *srcNode) {
    Node* destChild = destNode->newChild(child, &pool_);
    if (child->hasChild()) {
      copyAllChildren(child, destChild);
    }
  }
}

void GameTree::minusTree(GameTree* beMinusTree, const GameTree* minusTree) {
  beMinusTree->root_->minus(minusTree->root_);
  minusAllChildren(beMinusTree->root_, minusTree->root_);
}

void GameTree::minusAllChildren(Node* beMinusNode, const Node* minusNode) {
  for (Node* minusChild : *minusNode) {
    Node* beMinusChild = beMinusNode->child(minusChild->index());
    beMinusChild->minus(minusChild);

    if (minusChild->hasChild()) {
      minusAllChildren(beMinusChild, minusChild);
    }
  }
}

void GameTree::mergeTree(GameTree* tree) {
  currentNode_->merge(tree->root_);
  mergeAllChildren(currentNode_, tree->root_);
}

void GameTree::mergeAllChildren(Node* mergingNode, const Node* mergedNode) {
  for (Node* mergedChild : *mergedNode) {
    Node* mergingChild = mergingNode->child(mergedChild->index());
    if (mergingChild != NULL) {
      mergingChild->merge(mergedChild);
    } else {
      mergingChild = mergingNode->newChild(mergedChild, &pool_);
    }

    if (mergedChild->hasChild()) {
      mergeAllChildren(mergingChild, mergedChild);
    }
  }
}

int GameTree::play(int index) {
  int whoWin = currentBoard_->play(index);

  Node* child = currentNode_->child(index);

  if (child == NULL)
    child = currentNode_->newChild(index, whoWin, &pool_);

  // delete other children except the child that going to play
  currentNode_->deleteChildrenExcept(child, &pool_);
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
    child = currentNode_->newChild(index, 0, &pool_);

  // Delete other children except the child that going to play.
  currentNode_->deleteChildrenExcept(child, &pool_);
  currentNode_->clear();

  currentNode_ = child;
}

void GameTree::undo() {
  currentBoard_->undo(currentNode_->index());
  currentNode_ = currentNode_->parent();

  // Note: In some rare case, this node will leave some data
  // that child return when mcts, and may cause seg fault,
  // so clear this node's data is necessary.
  currentNode_->clear();

  // Delete all children.
  currentNode_->deleteChildren(&pool_);
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
