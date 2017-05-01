#include "gomoku/chesstype.hpp"
#include "gomoku/status.hpp"
#include "gomoku/freestyle/virtualboard.hpp"
#include "gametree.hpp"
#include "node.hpp"

#include <iostream>
#include <iomanip>

#ifdef DEBUG
#include "log.hpp"
#endif

GameTree::GameTree() {
  /* create the grand root(e.g. a root representing a blank board) */
  root = new Node();
  currentNode = root;
  currentBoard = new VirtualBoard();
}

GameTree::~GameTree() {
  delete root;
}

void GameTree::clear() {
  delete root;
  root = new Node();
  currentNode = root;

  delete currentBoard;
  currentBoard = new currentBoard();
}

void GameTree::MCTS(int maxCycle) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle; ++cycle) {
    if (currentNode->winning() || currentNode->losing()) return;

    VirtualBoard board(currentBoard);

    int result = selection(&node, &board);

    if (result == -2) {
      /* simulate only if child is not winning */
      result = simulation(&board);
    }

    if (result == -1)
      backProp(node);
    else
      backProp(node, result);
  }
}

void GameTree::MCTS(int maxCycle, bool &stop) {
  Node* node;

  for (int cycle = 0; cycle < maxCycle && stop == false; ++cycle) {
    if (currentNode->winning() || currentNode->losing()) return;

    VirtualBoard board(currentBoard);

    int result = selection(&node, &board);

    if (result == -2) {
      /* simulate only if child is not winning */
      result = simulation(&board);
    }

    if (result == -1)
      backProp(node);
    else
      backProp(node, result);

    if (cycle % 1000 == 0) std::cout << "\rbackground: " << cycle << std::flush;
  }
}

void GameTree::MCTSResult(int &row, int &col) {
  int mostTimes = -1;
  int score;

  int scoreSum = currentBoard->getScoreSum();

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL) {

        #ifdef DEBUG
        int playout = currentNode->childNode[r][c]->totalPlayout();
        double scorePer = (double)(currentBoard->getScore(r, c)) / scoreSum;
        double ucbValue = currentNode->getUCBValue(r, c);

        Log log;
        Log::precision(3);
        log << (char)(c + 65) << r + 1
            << "  sim: " << playout
            << "  WinR: " << currentNode->childNode[r][c]->winRate()
            << "  scoreP: "  << scorePer
            << "  UCB: " << ucbValue
            << "  scoreP + UCB: "
            << (scorePer + ucbValue)
            << "\n";
        #endif

        /* priority order: playout -> score */
        if (currentNode->childNode[r][c]->totalPlayout() > mostTimes) {
          row = r;
          col = c;
          mostTimes = currentNode->childNode[r][c]->totalPlayout();
          score = currentBoard->getScore(r, c);
        } else if (currentNode->childNode[r][c]->totalPlayout() == mostTimes) {
          if (currentBoard->getScore(r, c) > score) {
            row = r;
            col = c;
            score = currentBoard->getScore(r, c);
          }
        }
      }

  double scorePer = (double)(currentBoard->getScore(row, col)) / scoreSum;
  double ucbValue = currentNode->getUCBValue(row, col);

  std::cout << std::fixed << std::setprecision(3)
            << "total sim: " << currentNode->totalPlayout()
            << "  best point: "
            << (char)(col + 65) << row + 1
            << "  sim: " << currentNode->childNode[row][col]->totalPlayout()
            << "  WinR: " << currentNode->childNode[row][col]->winRate()
            << "  scoreP: "  << scorePer
            << "  UCB: " << ucbValue
            << "  scoreP + UCB: " << (scorePer + ucbValue)
            << "  winning: " << currentNode->winning()
            << "  losing: " << currentNode->losing()
            << std::endl;
}

int GameTree::selection(Node** node, VirtualBoard* board) {
  *node = currentNode;

  while (true) {
    int r, c;

    int result = (*node)->selection(r, c, board);
    if (result != -2) {
      return result;
    }

    /* check if reached leaf */
    if ((*node)->childNode[r][c] == NULL) {
      bool parentWinning = board->play(r, c);
      (*node)->childNode[r][c] = new Node(*node, r, c, parentWinning);
      *node = (*node)->childNode[r][c];

      if (parentWinning)
        return 0;
      else
        return -2;
    }

    *node = (*node)->childNode[r][c];
    board->play(r, c);
  }
}

int GameTree::simulation(VirtualBoard* board) {
  const int MAX_DEPTH = 50;
  /* simulate until reach max depth */
  for (int d = 1; d <= MAX_DEPTH; ++d) {
    int r, c;
    /* return tie(-1) if every point is not empty point */
    if (!board->getHSP(r, c))
      return -1;

    /* if win, return */
    if (board->play(r, c))
      return (d & 1);
  }

  return -1;
}

void GameTree::backProp(Node* node, bool result) {
  // note: cannot use do-while here
  while (node != currentNode) {
    node->update(result);
    node = node->getParent();
    result = !result;
  }
  node->update(result);
}

void GameTree::backProp(Node* node) {
  // note: cannot use do-while here
  while (node != currentNode) {
    node->update();
    node = node->getParent();
  }
  node->update();
}

bool GameTree::play(int row, int col) {
  bool winning = currentBoard->play(row, col);

  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col] = new Node(currentNode, row, col, winning);

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL) {
        if (r == row && c == col) continue;

        delete currentNode->childNode[r][c];
        currentNode->childNode[r][c] = NULL;
      }

  currentNode = currentNode->childNode[row][col];

  return winning;
}
