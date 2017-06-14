#include "gomoku/chesstype.hpp"
#include "gomoku/status.hpp"
#include "virtualboard.hpp"
#include "gametree.hpp"
#include "node.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <tuple>
#include <vector>

#ifdef ANALYZE
#include "log.hpp"
#endif

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
    for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
      for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
        if (currentNode->childNode[r][c] != NULL)
          if (currentNode->childNode[r][c]->totalPlayout() > mostTimes)
            mostTimes = currentNode->childNode[r][c]->totalPlayout();

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

void GameTree::MCTSResult(int* row, int* col) const {
  if (currentNode->winning()) {
    for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
      for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
        if (currentNode->childNode[r][c] != NULL)
          if (currentNode->childNode[r][c]->losing()) {
            *row = r; *col = c;
          }
  } else {
    int mostTimes = -1;
    int score;

    for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
      for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
        if (currentNode->childNode[r][c] != NULL) {

          /* priority order: playout -> score */
          if (currentNode->childNode[r][c]->totalPlayout() > mostTimes) {
            *row = r;
            *col = c;
            mostTimes = currentNode->childNode[r][c]->totalPlayout();
            score = currentBoard->getScore(r, c);
          } else if (currentNode->childNode[r][c]->totalPlayout() == mostTimes) {
            if (currentBoard->getScore(r, c) > score) {
              *row = r;
              *col = c;
              score = currentBoard->getScore(r, c);
            }
          }
        }
  }

  std::cout << std::fixed << std::setprecision(3)
            << "total sim: " << currentNode->totalPlayout()
            << "  best: " << (char)(*col + 65) << *row + 1
            << "  sim: " << currentNode->childNode[*row][*col]->totalPlayout()
            << "  WinR: " << currentNode->childNode[*row][*col]->winRate()
            << "  W: " << currentNode->winning()
            << "  L: " << currentNode->losing()
            << std::endl;

  #ifdef ANALYZE
  /* sort coordinates */
  std::vector<std::tuple<int, int>> sortedList;
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL)
        sortedList.push_back(std::make_tuple(r, c));

  std::sort(sortedList.begin(), sortedList.end(), [this](const std::tuple<int, int> i, const std::tuple<int, int> k)->bool{
    return currentNode->childNode[std::get<0>(i)][std::get<1>(i)]->totalPlayout()
      > currentNode->childNode[std::get<0>(k)][std::get<1>(k)]->totalPlayout();
  });

  Log log;
  Log::precision(3);
  log << "           sim    winRate      score       UCB    score + UCB\n";
  for (int i = 0; i < sortedList.size() ; ++i) {
    int *row = std::get<0>(sortedList[i]);
    int *col = std::get<1>(sortedList[i]);

    double winRate = currentNode->childNode[*row][*col]->winRate(),
           score = (double)(currentBoard->getScore(*row, *col)) / currentBoard->getScoreSum(),
           ucb = currentNode->getUCBValue(*row, *col);

    log << std::setw(2) << i + 1 << ". "
        << (char)('A' + *col) << std::setw(2) << *row + 1
        << "  " << std::setw(5) << currentNode->childNode[*row][*col]->totalPlayout()
        << "      " << std::setw(5) << winRate
        << "     " << std::setw(6) << score
        << "     " << std::setw(5) << ucb
        << "         " << std::setw(6) << score + ucb << "\n";
  }
  #endif
}

int GameTree::selection(Node** node, VirtualBoard* board) const {
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

int GameTree::simulation(VirtualBoard* board) const {
  const int MAX_DEPTH = 50;
  /* simulate until reach max depth */
  for (int d = 1; d <= MAX_DEPTH; ++d) {
    int r, c;
    /* return tie(-1) if every point is not empty point */
    if (!board->getHSP(&r, &c))
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

int GameTree::play(int row, int col) {
  int whoWin = currentBoard->play(row, col);

  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col] = new Node(currentNode, row, col, whoWin);

  currentNode = currentNode->childNode[row][col];

  return whoWin;
}

void GameTree::undo() {
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->parent()->childNode[r][c] == currentNode) {
        currentBoard->undo(r, c);
        currentNode = currentNode->parent();
        return;
      }
}
