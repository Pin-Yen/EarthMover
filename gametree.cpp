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
  delete root;
  delete currentBoard;
}

void GameTree::reset(VirtualBoard* board) {
  const int MAX_NODE = 200000;
  pool.free();
  pool.init(sizeof(Node) * MAX_NODE);

  if (root != NULL)
    delete root;
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
    for (int i = 0; i < CHILD_LENGTH; ++i)
      if (currentNode->childNode[i] != NULL)
        if (currentNode->childNode[i]->totalPlayout() > mostTimes)
          mostTimes = currentNode->childNode[i]->totalPlayout();

  } while (mostTimes < minMostTimesCycle);

}

void GameTree::MCTS(int maxCycle, const bool &stop) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle && stop == false; ++cycle) {
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
    for (int i = 0; i < CHILD_LENGTH; ++i)
      if (currentNode->childNode[i] != NULL)
        if (currentNode->childNode[i]->losing())
          index = i;

  } else {
    int mostTimes = -1;
    int score;

    for (int i = 0; i < CHILD_LENGTH; ++i)
      if (currentNode->childNode[i] != NULL) {

        /* priority order: playout -> score */
        if (currentNode->childNode[i]->totalPlayout() > mostTimes) {
          index = i;
          mostTimes = currentNode->childNode[i]->totalPlayout();
          score = currentBoard->getScore(i);
        } else if (currentNode->childNode[i]->totalPlayout() == mostTimes) {
          if (currentBoard->getScore(i) > score) {
            index = i;
            score = currentBoard->getScore(i);
          }
        }
      }

    /* pass*/
    if (mostTimes == -1) {
      std::cout << "pass" << std::endl;
      return -1;
    }
  }

  std::cout << std::fixed << std::setprecision(3)
            << "total sim: " << currentNode->totalPlayout()
            << "  best: " << (char)(index % 15 + 65) << index / 15 + 1
            << "  sim: " << currentNode->childNode[index]->totalPlayout()
            << "  WinR: " << currentNode->childNode[index]->winRate()
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
    if ((*node)->childNode[index] == NULL) {
      bool parentWinning = board->play(index);
      (*node)->childNode[index] = new Node(*node, parentWinning);
      *node = (*node)->childNode[index];

      if (parentWinning)
        return 0;
      else
        return -2;
    }

    *node = (*node)->childNode[index];
    board->play(index);
  }
}

int GameTree::simulation(VirtualBoard* board) const {
  const int MAX_DEPTH = 50;
  /* simulate until reach max depth */
  for (int d = 1; d <= MAX_DEPTH; ++d) {
    int index;
    /* return tie(-1) if every point is not empty point */
    if (!board->getHSP(&index))
      return -1;

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

  if (currentNode->childNode[index] == NULL)
    currentNode->childNode[index] = new Node(currentNode, whoWin);

  currentNode = currentNode->childNode[index];

  return whoWin;
}

void GameTree::pass() {
  currentBoard->pass();

  if (currentNode->childNode[CHILD_LENGTH] == NULL)
    currentNode->childNode[CHILD_LENGTH] = new Node(currentNode, 0);

  currentNode = currentNode->childNode[CHILD_LENGTH];
}

void GameTree::undo() {
  for (int i = 0; i < CHILD_LENGTH + 1; ++i)
    if (currentNode->parent()->childNode[i] == currentNode) {
      currentBoard->undo(i);
      currentNode = currentNode->parent();
      return;
    }
}

std::string GameTree::getTreeJSON() {
  if (currentNode == NULL) return "";
  if (currentNode->parent() == NULL) return "";

  /* get current node's index */
  for (int i = 0; i < CHILD_LENGTH + 1; ++i)
    if (currentNode->parent()->childNode[i] == currentNode)
      return getSubTreeJSON(currentNode, i, currentBoard->whoTurn()).dump();
}

json GameTree::getSubTreeJSON(Node* node, int position, bool whiteTurn) {
  json tree;

  tree["index"] = position;

  tree["totalCount"] = node->totalPlayout();

  tree["winRate"] = whiteTurn ? node->winRate() : 1 - node->winRate();
  tree["isWinning"] = whiteTurn ? node->winning() : node->losing();
  tree["isLosing"] = whiteTurn ? node->losing() : node->winning();

  tree["whiteTurn"] = whiteTurn;

  tree["children"] = json::array();
  for (int i = 0; i < CHILD_LENGTH; ++i) {
    if (node->childNode[i] != NULL && node->childNode[i]->totalPlayout() >= 8) {
      tree["children"].push_back(getSubTreeJSON(node->childNode[i], i, !whiteTurn));
    }
  }
  return tree;
}