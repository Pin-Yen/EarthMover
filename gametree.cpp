#include "gomoku/freestyle/virtualboard.hpp"
#include "gametree.hpp"
#include "node.hpp"

#include <iostream>
#include <iomanip>

GameTree::GameTree() {
  /* create the grand root(e.g. a root representing a blank board) */
  root = new Node();
  currentNode = root;
}

GameTree::~GameTree() {
  delete root;
}

void GameTree::MCTS(int &row, int &col, int maxCycle) {
  const int SIMULATE_DEPTH = 50;

  Node* node;
  for (int cycle = 0; cycle < maxCycle; ++cycle) {

    int result = selection(&node);

    if (result == -2) {
      /* simulate only if child is not winning */
      result = simulation(node, SIMULATE_DEPTH);
    }

    backProp(node, result);
  }

  // return the point that select most times
  int mostTimes = -1;

  int whoTurn = currentNode->getWhoTurn();
  int scoreSum = currentNode->board->getScoreSum(whoTurn);

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL) {

        /*
        // debugger
        int playout = currentNode->childNode[r][c]->getTotalPlayout();
        double scorePer = (double)(currentNode->board->getScore(r, c, whoTurn)) / scoreSum;
        double ucbValue = currentNode->getUCBValue(r, c, whoTurn);

        std::cout << std::fixed << std::setprecision(3)
                  << (char)(c + 65) << r + 1
                  << "  sim: " << playout
                  << "  BWinP: " << currentNode->childNode[r][c]->getWinRate(false)
                  << "  WWinP: " << currentNode->childNode[r][c]->getWinRate(true)
                  << "  scoreP: "  << scorePer
                  << "  UCB: " << ucbValue
                  << "  scoreP + UCB: "
                  << (scorePer *
                     std::max(0.1, ((1000 - playout) / 1000.0) * 0.9 + 0.1) +
                     ucbValue *
                     std::min(0.9, (playout / 1000.0) * 0.9 + 0.1))
                  << std::endl;
        // end debugger
        */

        if (currentNode->childNode[r][c]->getTotalPlayout() > mostTimes) {
          // ?? should we pick a random point if there are multiple best points,
          // or just pick the first point ??
          row = r;
          col = c;
          mostTimes = currentNode->childNode[r][c]->getTotalPlayout();
        }
      }

  // debugger
    int playout = currentNode->childNode[row][col]->getTotalPlayout();
    double scorePer = (double)(currentNode->board->getScore(row, col, whoTurn)) / scoreSum;
    double ucbValue = currentNode->getUCBValue(row, col, whoTurn);

    std::cout << std::fixed << std::setprecision(3)
              << "best point: "
              << (char)(col + 65) << row + 1
              << "  sim: " << playout
              << "  BWinP: " << currentNode->childNode[row][col]->getWinRate(false)
              << "  WWinP: " << currentNode->childNode[row][col]->getWinRate(true)
              << "  scoreP: "  << scorePer
              << "  UCB: " << ucbValue
              << "  scoreP + UCB: "
              << (scorePer *
                  std::max(0.1, ((1000 - playout) / 1000.0) * 0.9 + 0.1) +
                  ucbValue *
                  std::min(0.9, (playout / 1000.0) * 0.9 + 0.1))
              << std::endl;
  // end debugger

  /* destruct all child node */
  /* note: if want to keep the calculation, then should not call this */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL) {
        delete currentNode->childNode[r][c];
        currentNode->childNode[r][c] = NULL;
      }

  currentNode->clearPlayout();
}

int GameTree::selection(Node** selectedLeaf) {
  Node* node = currentNode;

  while (true) {
    int r, c;
    /* if every point is not empty point */
    if (!node->selection(r, c)) {
      *selectedLeaf = node;
      return -1;
    }

    /* handle if already win when playing at child */
    if (node->getIsSelfWinning()) {
      *selectedLeaf = node;
      return node->getWhoTurn();
    }

    /* check if reach leaf */
    if (node->childNode[r][c] == NULL) {
      node->childNode[r][c] = new Node(node, r, c);
      *selectedLeaf = node->childNode[r][c];
      return -2;
    }

    node = node->childNode[r][c];
  }
}

int GameTree::simulation(Node* node, int maxDepth) {
  return node->simulation(maxDepth);
}

void GameTree::backProp(Node* node, int result) {
  // note: cannot use do-while here
  while (node != currentNode) {
    node->update(result);
    node = node->getParent();
  }
  node->update(result);
}

bool GameTree::play(int row, int col) {
  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col] = new Node(currentNode, row, col);

  currentNode = currentNode->childNode[row][col];

  return currentNode->getIsSelfWinning();
}
