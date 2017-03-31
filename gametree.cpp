#include "gomoku/freestyle/virtualboard.hpp"
#include "gomoku/freestyle/status.hpp"
#include "gomoku/freestyle/chesstype.hpp"
#include "gametree.hpp"
#include "node.hpp"

#include <iostream>
#include <iomanip>

#include "gomoku/freestyle/typetree.hpp"

#ifdef DEBUG
#include "log.hpp"
#endif

GameTree::GameTree() {
  TypeTree::initialize();

  /* create the grand root(e.g. a root representing a blank board) */
  root = new Node();
  currentNode = root;
  currentBoard = new VirtualBoard();
}

GameTree::~GameTree() {
  delete root;
}

void GameTree::MCTS(int &row, int &col, int maxCycle) {
  const int SIMULATE_DEPTH = 50;

  Node* node;

  for (int cycle = 0; cycle < maxCycle; ++cycle) {
    VirtualBoard board(currentBoard);

    int result = selection(&node, &board);

    if (result == -2) {
      /* simulate only if child is not winning */
      result = simulation(node, SIMULATE_DEPTH, &board);
    }

    backProp(node, result);
  }

  // return the point that select most times
  int mostTimes = -1;
  int score;

  int whoTurn = currentBoard->getWhoTurn();
  int scoreSum = currentBoard->getScoreSum(whoTurn);

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL) {

        #ifdef DEBUG
        int playout = currentNode->childNode[r][c]->getTotalPlayout();
        double scorePer = (double)(currentBoard->getScore(r, c, whoTurn)) / scoreSum;
        double ucbValue = currentNode->getUCBValue(r, c, whoTurn);

        Log log;
        Log::precision(3);
        log << (char)(c + 65) << r + 1
            << "  sim: " << playout
            << "  BWinP: " << currentNode->childNode[r][c]->getWinRate(false)
            << "  WWinP: " << currentNode->childNode[r][c]->getWinRate(true)
            << "  scoreP: "  << scorePer
            << "  UCB: " << ucbValue
            << "  scoreP + UCB: "
            << (scorePer + ucbValue)
            << "\n";
        #endif

        /* priority order: playout -> score */
        if (currentNode->childNode[r][c]->getTotalPlayout() > mostTimes) {
          row = r;
          col = c;
          mostTimes = currentNode->childNode[r][c]->getTotalPlayout();
          score = currentBoard->getScore(r, c, whoTurn);
        } else if (currentNode->childNode[r][c]->getTotalPlayout() == mostTimes) {
          if (currentBoard->getScore(r, c, whoTurn) > score) {
            row = r;
            col = c;
            score = currentBoard->getScore(r, c, whoTurn);
          }
        }
      }

  int playout = currentNode->childNode[row][col]->getTotalPlayout();
  double scorePer = (double)(currentBoard->getScore(row, col, whoTurn)) / scoreSum;
  double ucbValue = currentNode->getUCBValue(row, col, whoTurn);

  std::cout << std::fixed << std::setprecision(3)
            << "best point: "
            << (char)(col + 65) << row + 1
            << "  sim: " << playout
            << "  BWinP: " << currentNode->childNode[row][col]->getWinRate(false)
            << "  WWinP: " << currentNode->childNode[row][col]->getWinRate(true)
            << "  scoreP: "  << scorePer
            << "  UCB: " << ucbValue
            << "  scoreP + UCB: " << (scorePer + ucbValue)
            << "  winning: " << currentNode->childNode[row][col]->winning()
            << "  losing: " << currentNode->childNode[row][col]->losing()
            << std::endl;

  /* destruct all child node */
  /* note: if want to keep the calculation, then should not call this */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL) {
        delete currentNode->childNode[r][c];
        currentNode->childNode[r][c] = NULL;
      }

  currentNode->clearPlayout();
  currentNode->clearWinLose();
}

int GameTree::selection(Node** selectedLeaf, VirtualBoard* board) {
  Node* node = currentNode;

  while (true) {
    int r, c;
    /* if there is no point can select */
    if (!node->selection(r, c, board)) {
      *selectedLeaf = node;
      if (node->winning()) {
        /* if no point can select is because of every point is losing */
        return board->getWhoTurn();
      }
      if (node->losing()) {
        /* if no point can select is because of already winning */
        return !board->getWhoTurn();
      }
      return -1;
    }

    /* check if reach leaf */
    if (node->childNode[r][c] == NULL) {
      bool parentWinning = board->play(r, c);
      node->childNode[r][c] = new Node(node, r, c, parentWinning);
      *selectedLeaf = node->childNode[r][c];

      if (parentWinning)
        return !board->getWhoTurn();
      else
        return -2;
    }

    node = node->childNode[r][c];
    board->play(r, c);
  }
}

int GameTree::simulation(Node* node, int maxDepth, VirtualBoard* board) {
  return node->simulation(maxDepth, board);
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
  bool winning = currentBoard->play(row, col);

  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col] =
      new Node(currentNode, row, col, !winning);

  currentNode = currentNode->childNode[row][col];

  return winning;
}
