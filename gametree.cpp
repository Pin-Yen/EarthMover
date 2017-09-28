#include "gametree.h"

#include <assert.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>
#include <tuple>
#include <vector>

#include "const.h"
#include "gomoku/chesstype.h"
#include "lib/json.h"
#include "memorypool.h"
#include "node.h"
#include "virtualboard.h"

using json = nlohmann::json;

GameTree::GameTree()
    : root_(NULL),
      currentNode_(NULL),
      currentBoard_(NULL),
      rave_(NULL) { }

GameTree::~GameTree() {
  pool_.free();
  if (currentBoard_ != NULL) delete currentBoard_;
  if (rave_ != NULL) delete rave_;
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

  rave_ = new RAVE[currentBoard_->length() * 2];
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

    bool whoTurn = board->whoTurn();

    backProp(node, status, whoTurn);

    delete board;
  }
  return true;
}

void GameTree::mcts(int batch, int minCount) {
  while (true) {
    if (!mcts(batch)) return;

    // get mostTimes
    int mostTimes = 0;

    for (Node* child : *currentNode_) {
      if (child->count() > mostTimes) {
        mostTimes = child->count();
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
      if (child->count() > mostTimes) {
        index = child->index();
        mostTimes = child->count();
        score = currentBoard_->getScore(index);
      } else if (child->count() == mostTimes) {
        if (currentBoard_->getScore(child->index()) > score) {
          index = child->index();
          mostTimes = child->count();
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
            << "total sim: " << currentNode_->count()
            << "  best: "
            << static_cast<char>(index % 15 + 65) << index / 15 + 1
            << "  sim: " << child->count()
            << "  WinR: " << child->winRate()
            << "  W/L: " << currentNode_->gameStatus()
            << std::endl;

  return index;
}

std::pair<SearchStatus, GameTree::Node*> GameTree::selection(
    VirtualBoard* board) {
  Node* node = currentNode_;

  while (true) {
    std::pair<SearchStatus, Node*> result =
        node->selection(board, rave_ + board->whoTurn() * board->length(), &pool_);
    if (result.first != UNKNOWN) return result;
    node = result.second;
  }
}

SearchStatus GameTree::simulation(VirtualBoard* board) const {
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

void GameTree::backProp(Node* node, SearchStatus result, bool whoTurn) {
  while (node != currentNode_) {
    node->update(result);
    rave_[whoTurn * node->index()].update(result);
    node = node->parent();
    // reverse result (WIN <-> LOSE)
    result = static_cast<SearchStatus>(-static_cast<int>(result));
    whoTurn = !whoTurn;
  }
  node->update(result);
}

void GameTree::copy(const GameTree* source) {
  const int MAX_NODE = 320000;
  pool_.init(sizeof(Node), MAX_NODE);

  root_ = new(&pool_) Node(source->currentNode_->parent(), source->currentNode_);
  currentNode_ = root_;
  currentBoard_ = source->currentBoard_->clone();

  rave_ = new RAVE[currentBoard_->length() * 2];
  for (int i = 0; i < currentBoard_->length() * 2; ++i) {
    rave_[i] = source->rave_[i];
  }

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
  for (int i = 0; i < currentBoard_->length() * 2; ++i) {
    beMinusTree->rave_[i].minus(&(minusTree->rave_[i]));
  }
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
  for (int i = 0; i < currentBoard_->length() * 2; ++i) {
    rave_[i].merge(&(tree->rave_[i]));
  }
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

GameStatus GameTree::play(int index) {
  GameStatus status = currentBoard_->play(index);

  Node* child = currentNode_->child(index);

  if (child == NULL)
    child = currentNode_->newChild(index, status, &pool_);

  // delete other children except the child that going to play
  currentNode_->deleteChildrenExcept(child, &pool_);
  currentNode_->clear();

  currentNode_ = child;

  return status;
}

void GameTree::pass() {
  // get the index of pass
  int index = currentBoard_->pass();

  // return for not allow to pass
  if (index == -1) return;

  Node* child = currentNode_->child(index);

  if (child == NULL)
    child = currentNode_->newChild(index, NOTHING, &pool_);

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

  tree["totalCount"] = node->count();

  tree["winRate"] = whoTurn ? node->winRate() : 1 - node->winRate();
  tree["winOrLose"] = whoTurn ? node->gameStatus() : -node->gameStatus();

  tree["whoTurn"] = whoTurn;

  tree["children"] = json::array();

  for (Node* child : *node) {
    if (child->count() >= 8) {
      tree["children"].push_back(getSubTreeJSON(child, !whoTurn));
    }
  }
  return tree;
}
