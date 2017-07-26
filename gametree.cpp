#include "gomoku/chesstype.hpp"
#include "gomoku/status.hpp"
#include "virtualboard.hpp"
#include "memorypool.hpp"
#include "gametree.hpp"
#include "node.hpp"
#include "lib/json.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <vector>

using json = nlohmann::json;

MemoryPool GameTree::pool_;

GameTree::GameTree() {
  root_ = NULL;
  currentNode_ = NULL;
  currentBoard_ = NULL;
}

GameTree::~GameTree() {
  pool_.free();
  delete currentBoard_;
}

void GameTree::reset(VirtualBoard* board) {
  const int MAX_NODE = 2000000;
  pool_.free();
  pool_.init(sizeof(Node) * MAX_NODE);
  root_ = new Node();

  currentNode_ = root_;

  if (currentBoard_ != NULL)
    delete currentBoard_;
  currentBoard_ = board->create();
}

void GameTree::MCTS(int maxCycle) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle; ++cycle) {
    if (!currentNode_->notWinOrLose()) return;

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
}

void GameTree::MCTS(int minCycle, int minMostTimesCycle) {
  Node* node;

  int mostTimes = -1;

  do {
    for (int cycle = 0; cycle < minCycle; ++cycle) {
      if (!currentNode_->notWinOrLose()) return;

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

    // get mostTimes
    for (Node* child : *currentNode_) {
      if (child->totalPlayout() > mostTimes)
          mostTimes = child->totalPlayout();
    }
  } while (mostTimes < minMostTimesCycle);
}

void GameTree::MCTS(int maxCycle, const bool &stop) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle && stop == false; ++cycle) {
    if (!currentNode_->notWinOrLose()) return;

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

    if (cycle % 2000 == 0) std::cout << "\rbackground: " << cycle << std::flush;
  }
}

int GameTree::MCTSResult() const {
  int index;

  // if is winning, select it
  if (currentNode_->winning()) {
    for (Node* child : *currentNode_) {
      if (child->losing()) index = child->index();
    }
  } else {
    // select child that has max playout
    int mostTimes = -1;

    for (Node* child : *currentNode_) {
      // priority order: playout -> score
      if (child->totalPlayout() > mostTimes) {
        index = child->index();
        mostTimes = child->totalPlayout();
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
            << "  best: " << (char)(index % 15 + 65) << index / 15 + 1
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

int GameTree::play(int index) {
  int whoWin = currentBoard_->play(index);

  Node* child = currentNode_->child(index);

  if (child == NULL)
    child = currentNode_->newChild(index, whoWin);

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

  currentNode_ = child;
}

void GameTree::undo() {
  currentBoard_->undo(currentNode_->index());
  currentNode_ = currentNode_->parent();
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