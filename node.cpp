#include "gomoku/freestyle/virtualboard.hpp"
#include "gametree.hpp"
#include "node.hpp"

#include <iostream>
#include <cmath>

#ifdef DEBUG
#include "objectcounter.hpp"
#endif

GameTree::Node::Node() {
  /* initialize all childNodes to NULL */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  /* initialize playout counters to 0 */
  for (int i = 0; i < 3; ++i)
    playout[i] = 0;

  /* initiaize parent node */
  parent = NULL;

  winning_ = false;
  losing_ = false;

  #ifdef DEBUG
  ObjectCounter::registerNode();
  #endif
}

GameTree::Node::Node(Node *parentNode, int row, int col, bool losing) {
  /* initialize all childNodes to NULL */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  /* initialize playout counters to 0 */
  for (int i = 0 ; i < 3; ++i)
    playout[i] = 0;

  /* initiaize parent node */
  parent = parentNode;

  winning_ = false;
  losing_ = losing;

  /* if is losing, set parent to winning */
  if (losing)
    parent->winning_ = true;

  #ifdef DEBUG
  ObjectCounter::registerNode();
  #endif
}

GameTree::Node::~Node() {
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (childNode[r][c] != NULL)
        delete childNode[r][c];

  #ifdef DEBUG
  ObjectCounter::unregisterNode();
  #endif
}

void GameTree::Node::update(int result) {
  // update total playout
  ++playout[2];
  // if result not tie, update the winner's playout
  if (result != -1) ++playout[result];
}

int GameTree::Node::selection(int &row, int &col, VirtualBoard* board) {
  bool whoTurn = board->whoTurn();

  if (winning_) return whoTurn;

  /* current max value */
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();
  int same = 1;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      int score = board->getScore(r, c);

      /* skip if this point is not empty */
      if (score == -1) continue;

      if (childNode[r][c] != NULL) {
        /* if child node is winning then DO NOT select this point */
        if (childNode[r][c]->winning_) {
          childWinning = true;
          continue;
        }

        /* if exist 100% win, select this point */
        if (childNode[r][c]->winRate(whoTurn) == 1) {
          row = r; col = c;
          return -2;
        }
      }

      double ucbValue = getUCBValue(r, c, whoTurn);

      double value = ((double)score / scoreSum) + ucbValue;

      if (value > max) {
        same = 1;

        max = value;
        row = r; col = c;
      } else if (value == max) {
        ++same;
        if (((double)rand() / RAND_MAX) <= (1. / same)) {
          row = r; col = c;
        }
      }
    }

  // TODO: if every child is winning point, set this point to losing
  if (max == -1) {
    if (childWinning) {
      losing_ = true;
      parent->winning_ = true;

      return !whoTurn;
    }

    /* return false if every point is not empty */
    return -1;
  }

  return -2;
}

double GameTree::Node::getUCBValue(int r, int c, bool color) {
  if (playout[2] == 0)
    return 0;

  const int CON = 1;

  if (childNode[r][c] != NULL) {
    return (childNode[r][c]->winRate(color) +
            sqrt(CON * log10(playout[2]) / (1 + childNode[r][c]->totalPlayout())));
  }
  else {
    return (sqrt(CON * log10(playout[2]) / 1));
  }
}