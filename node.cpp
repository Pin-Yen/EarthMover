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

  /* initialize winning flags */
  //isChildWinning = false;
  //isSelfWinning = false;

  winning_ = false;
  losing_ = false;

  #ifdef DEBUG
  ObjectCounter::registerNode();
  #endif
}

GameTree::Node::Node(Node *parentNode, int row, int col, bool winning) {
  /* initialize all childNodes to NULL */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  //isChildWinning = false;

  /* initialize playout counters to 0 */
  for (int i = 0 ; i < 3; ++i)
    playout[i] = 0;

  /* initiaize parent node */
  parent = parentNode;

  /* set isWinning*/
  //isSelfWinning = isWinning;

  winning_ = winning;
  losing_ = false;

  /* if is winning, set parent's winning child */
  /*if (isSelfWinning) {
    parent->isChildWinning = true;
    parent->winningChildRow = row;
    parent->winningChildCol = col;
  }*/
  if (winning) {
    parent->losing_ = true;
  }

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

bool GameTree::Node::selection(int &row, int &col, VirtualBoard* board) {
  /* if there's a winning child then return it */
  /*if (isChildWinning) {
    row = winningChildRow;
    col = winningChildCol;
    return true;
  }*/


  bool whoTurn = board->getWhoTurn();

  // current max value
  double max = -1;

  bool childLosing = false;

  int scoreSum = board->getScoreSum(whoTurn);
  int same = 1;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {

      int score = board->getScore(r, c, whoTurn);
      /* skip if this point is not empty */
      if (score == -1) continue;

      // TODO: avoid select a losing point
      if (childNode[r][c] != NULL) {
        if (childNode[r][c]->losing_) {
          childLosing = true;
          continue;
        }
      }

      double ucbValue = getUCBValue(r, c, whoTurn);

      double value = ((double)score / scoreSum) + ucbValue;

      if (value > max) {
        same = 1;

        max = value;
        row = r; col = c;
      }
      else if (value == max) {
        ++same;
        if (((double)rand() / RAND_MAX) <= (1. / same)) {
          row = r; col = c;
        }
      }
    }

  // TODO: if every child is losing point, set this point to winning
  if (max == -1) {
    if (childLosing) {
      winning_ = true;
      parent->losing_ = true;
    }

    /* return false if every point is not empty */
    return false;
  }

  return true;
}

int GameTree::Node::simulation(int maxDepth, VirtualBoard* board) {
  /* simulate until reach max depth */
  for (int d = 0; d < maxDepth; ++d) {
    int r, c;
    /* return tie(-1) if every point is not empty point */
    if (!board->getHSP(r, c))
      return -1;

    /* if win, return who win */
    if (board->play(r, c))
      return board->getWhoTurn();
  }

  return -1;
}

double GameTree::Node::getUCBValue(int r, int c, bool color) {
  if (playout[2] == 0)
    return 0;

  const float con = 0.5;

  if (childNode[r][c] != NULL) {
    return (childNode[r][c]->getWinRate(color) +
            sqrt(con * log10(playout[2]) / (1 + childNode[r][c]->getTotalPlayout())));
  }
  else {
    return (sqrt(con * log10(playout[2]) / 1));
  }
}