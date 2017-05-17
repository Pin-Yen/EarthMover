#include "gomoku/chesstype.hpp"
#include "gomoku/status.hpp"
#include "virtualboard.hpp"
#include "gomoku/virtualboard.hpp"
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

GameTree::Node::Node(Node *parentNode, int row, int col, int parentWinOrLose) {
  /* initialize all childNodes to NULL */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  /* initialize playout counters to 0 */
  for (int i = 0 ; i < 3; ++i)
    playout[i] = 0;

  /* initiaize parent node */
  parent = parentNode;

  switch (parentWinOrLose) {
    case 1:
      winning_ = false;
      losing_ = true;
      break;
    case -1:
      winning_ = true;
      losing_ = false;
      break;
    default:
      winning_ = false;
      losing_ = false;
  }

  /* if is losing, set parent to winning */
  if (losing_)
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

int GameTree::Node::selection(int &row, int &col, VirtualBoard* board) {
  if (winning_) return 1;
  if (losing_) return 0;

  /* current max value */
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();
  int same = 1;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      int score = board->getScore(r, c);

      /* skip if this point is occupied */
      if (score == -1) continue;

      if (childNode[r][c] != NULL) {
        /* if child node is winning then DO NOT select this point */
        if (childNode[r][c]->winning_) {
          childWinning = true;
          continue;
        }

        /* if there exists a point that wins in all previous simulations, select this point */
        if (childNode[r][c]->winRate() == 1) {
          row = r; col = c;
          return -2;
        }
      }

      double ucbValue = getUCBValue(r, c);

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

  /* if no point can select */
  if (max == -1) {
    /* if every child wins, mark this point as a losing point */
    if (childWinning) {
      losing_ = true;
      parent->winning_ = true;

      return 0;
    }

    /* if the chessboard is FULL */
    return -1;
  }

  return -2;
}

double GameTree::Node::getUCBValue(int r, int c) {
  if (playout[2] == 0)
    return 0;

  if (childNode[r][c] != NULL) {
    return (childNode[r][c]->winRate() +
            sqrt(0.2 * log10(playout[2]) / (1 + childNode[r][c]->totalPlayout())));
  } else {
    return (sqrt(0.2 * log10(playout[2]) / 1));
  }
}