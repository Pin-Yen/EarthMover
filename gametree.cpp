#include "gomoku/chesstype.hpp"
#include "gomoku/status.hpp"
#include "virtualboard.hpp"
#include "memorypool.hpp"
#include "gametree.hpp"
#include "node.hpp"
#include  "lib/json.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <vector>

#ifdef ANALYZE
#include "log.hpp"
#endif

using json = nlohmann::json;

MemoryPool GameTree::pool;

GameTree::GameTree() {
  root = NULL;
  currentNode = NULL;
  currentBoard = NULL;
}

GameTree::~GameTree() {
  pool.free();
  delete currentBoard;
}

void GameTree::reset(VirtualBoard* board) {
  const int MAX_NODE = 1000000;
  pool.free();
  pool.init(sizeof(Node) * MAX_NODE);
  root = new Node();

  currentNode = root;

  if (currentBoard != NULL)
    delete currentBoard;
  currentBoard = board->create();
}

void GameTree::MCTS(int maxCycle) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle; ++cycle) {
    if (currentNode->winning() || currentNode->losing()) return;

    VirtualBoard* board = currentBoard->clone();

    int result = selection(&node, board);

    if (result == -2) {
      /* simulate only if child is not winning */
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
      if (currentNode->winning() || currentNode->losing()) return;

      VirtualBoard* board = currentBoard->clone();

      int result = selection(&node, board);

      if (result == -2) {
        /* simulate only if child is not winning */
        result = simulation(board);
      }

      if (result == -1)
        backProp(node);
      else
        backProp(node, result);

      delete board;
    }

    /* get mostTimes */
    for (Node* child : *currentNode) {
      if (child->totalPlayout() > mostTimes)
          mostTimes = child->totalPlayout();
    }

  } while (mostTimes < minMostTimesCycle);

}

void GameTree::MCTS(int maxCycle, bool* continueThinking) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle && continueThinking; ++cycle) {
    if (currentNode->winning() || currentNode->losing()) return;

    VirtualBoard* board = currentBoard->clone();

    int result = selection(&node, board);

    if (result == -2) {
      /* simulate only if child is not winning */
      result = simulation(board);
    }

    if (result == -1)
      backProp(node);
    else
      backProp(node, result);

    delete board;

    if (cycle % 1000 == 0) std::cout << "\rbackground: " << cycle << std::flush;
  }
}

int GameTree::MCTSResult() const {
  int index;

  if (currentNode->winning()) {
    for (Node* child : *currentNode) {
      if (child->losing()) index = child->index();
    }
  } else {
    int mostTimes = -1;
    int score;

    for (Node* child : *currentNode) {
      // priority order: playout -> score
      if (child->totalPlayout() > mostTimes) {
        index = child->index();
        mostTimes = child->totalPlayout();
        score = currentBoard->getScore(index);
      } else if (child->totalPlayout() == mostTimes) {
        if (currentBoard->getScore(child->index()) > score) {
          index = child->index();
          score = currentBoard->getScore(index);
        }
      }
    }

    /* pass*/
    if (mostTimes == -1) {
      std::cout << "pass" << std::endl;
      return -1;
    }
  }

  Node* child = currentNode->child(index);

  std::cout << std::fixed << std::setprecision(3)
            << "total sim: " << currentNode->totalPlayout()
            << "  best: " << (char)(index % 15 + 65) << index / 15 + 1
            << "  sim: " << child->totalPlayout()
            << "  WinR: " << child->winRate()
            << "  W: " << currentNode->winning()
            << "  L: " << currentNode->losing()
            << std::endl;

  return index;
}

int GameTree::selection(Node** node, VirtualBoard* board) const {
  *node = currentNode;

  while (true) {
    int index;

    int result = (*node)->selection(&index, board);
    if (result != -2) {
      return result;
    }

    /* check if reached leaf */
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
  /* simulate until reach max depth */
  for (int d = 1; d <= MAX_DEPTH; ++d) {
    int index = board->getHSI();
    /* return tie(-1) if no useful point */
    if (index == -1) return -1;

    /* if win, return */
    if (board->play(index))
      return (d & 1);
  }

  return -1;
}

void GameTree::backProp(Node* node, bool result) {
  // note: cannot use do-while here
  while (node != currentNode) {
    node->update(result);
    node = node->parent();
    result = !result;
  }
  node->update(result);
}

void GameTree::backProp(Node* node) {
  // note: cannot use do-while here
  while (node != currentNode) {
    node->update();
    node = node->parent();
  }
  node->update();
}

int GameTree::play(int index) {
  int whoWin = currentBoard->play(index);

  Node* child = currentNode->child(index);

  if (child == NULL)
    child = currentNode->newChild(index, whoWin);

  assert(child != NULL);
  currentNode = child;

  return whoWin;
}

void GameTree::pass() {
  currentBoard->pass();

  Node* child = currentNode->child(CHILD_LENGTH);

  if (child == NULL)
    child = currentNode->newChild(CHILD_LENGTH, 0);

  currentNode = child;
}

void GameTree::undo() {
  currentBoard->undo(currentNode->index());
  currentNode = currentNode->parent();
}

std::string GameTree::getTreeJSON() {
  if (currentNode == NULL) return "";
  if (currentNode->parent() == NULL) return "";

  return getSubTreeJSON(currentNode, currentBoard->whoTurn()).dump();
}

json GameTree::getSubTreeJSON(Node* node, bool whiteTurn) {
  json tree;

  tree["index"] = node->index();

  tree["totalCount"] = node->totalPlayout();

  tree["winRate"] = whiteTurn ? node->winRate() : 1 - node->winRate();
  tree["isWinning"] = whiteTurn ? node->winning() : node->losing();
  tree["isLosing"] = whiteTurn ? node->losing() : node->winning();

  tree["whiteTurn"] = whiteTurn;

  tree["children"] = json::array();

  for (Node* child : *node) {
    if (child->totalPlayout() >= 8) {
      tree["children"].push_back(getSubTreeJSON(child, !whiteTurn));
    }
  }
  return tree;
}