#include "gomoku/freestyle/virtualboard.hpp"
#include "gametree.hpp"
#include "node.hpp"
#include "objectcounter.hpp"

#include <iostream>
#include <cmath>

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
  isChildWinning = false;
  isSelfWinning = false;

  ObjectCounter::registerNode();
}

GameTree::Node::Node(Node *parentNode, int row, int col, bool isWinning) {
  /* initialize all childNodes to NULL */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  isChildWinning = false;

  /* initialize playout counters to 0 */
  for (int i = 0 ; i < 3; ++i)
    playout[i] = 0;

  /* initiaize parent node */
  parent = parentNode;

  /* set isWinning*/
  isSelfWinning = isWinning;

  /* if is winning, set parent's winning child */
  if (isSelfWinning) {
    parent->isChildWinning = true;
    parent->winningChildRow = row;
    parent->winningChildCol = col;
  }

  ObjectCounter::registerNode();
}

GameTree::Node::~Node() {
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (childNode[r][c] != NULL)
        delete childNode[r][c];
  //delete board;
  ObjectCounter::unregisterNode();
}

void GameTree::Node::update(int result) {
  // update total playout
  ++playout[2];
  // if result not tie, update the winner's playout
  if (result != -1) ++playout[result];
}

bool GameTree::Node::selection(int &row, int &col, VirtualBoard* board) {
  /* if there's a winning child then return it */
  if (isChildWinning) {
    row = winningChildRow;
    col = winningChildCol;
    return true;
  }

  bool whoTurn = board->getWhoTurn();

  // current max value
  double max = -1;

  int scoreSum = board->getScoreSum(whoTurn);
  int same = 1;

  const double WEIGHT = 1000.0;
  const double BOUND_MAX = 0.9, BOUND_MIN = 1 - BOUND_MAX;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c) {
      int score = board->getScore(r, c, whoTurn);
      /* skip if this point is not empty */
      if (score == -1) continue;

      int playout = (childNode[r][c] == NULL ? 0 : childNode[r][c]->getTotalPlayout());
      double ucbValue = getUCBValue(r, c, whoTurn);

      double value = ((double)score / scoreSum) *
                     std::max(BOUND_MIN, ((WEIGHT - playout) / WEIGHT) * BOUND_MAX + BOUND_MIN) +
                     ucbValue *
                     std::min(BOUND_MAX, (playout / WEIGHT) * BOUND_MAX + BOUND_MIN);

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

  /* return false if every point is not empty */
  return (max > -1);
}

int GameTree::Node::simulation(int maxDepth, VirtualBoard* board) {
  //VirtualBoard* simulationBoard = new VirtualBoard(board);

  /* simulate until reach max depth */
  for (int d = 0; d < maxDepth; ++d) {
    int r, c;
    /* return tie(-1) if every point is not empty point */
    //if (!simulationBoard->getHSP(r, c)) {
    //  delete simulationBoard;
    if (!board->getHSP(r, c)) {
      return -1;
    }

    /* if win, return who win */
    //if (simulationBoard->play(r, c)) {
    //  int result = simulationBoard->getWhoTurn();
    //  delete simulationBoard;
    if (board->play(r, c)) {
      return board->getWhoTurn();
    }
  }
  //delete simulationBoard;
  return -1;
}

double GameTree::Node::getUCBValue(int r, int c, bool color) {
  if (playout[2] == 0)
    return 0;

  if (childNode[r][c] != NULL) {
    return (childNode[r][c]->getWinRate(color) +
            sqrt(2 * log(playout[2]) / (1 + childNode[r][c]->getTotalPlayout())));
  }
  else {
    return (sqrt(2 * log(playout[2]) / 1));
  }
}