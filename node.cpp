#include "node.hpp"
#include "gametree.hpp"
#include <cmath>

GameTree::Node::Node()
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  /* initialize playout counters to 0 */
  for (int i = 0; i < 3; ++i)
    playout[i] = 0;

  /* initialize board */
  board = new VirtualBoard();
  
  /* initiaize parent node */
  parent = NULL;
}

GameTree::Node::Node(Node *parentNode, int row, int col)
{
  /* initialize all childNodes to NULL, all childProp to 0 */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      childNode[r][c] = NULL;

  /* initialize playout counters to 0 */
  for(int i = 0 ; i < 3; ++i)
    playout[i] = 0;

  /* initialize board */
  board = new VirtualBoard(parentNode->board);
  board->play(row, col);

  /* initiaize parent node */
  parent = parentNode;
}

GameTree::Node::~Node()
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (childNode[r][c] != NULL)
        delete childNode[r][c];
}

void GameTree::Node::update(int result)
{
  ++playout[2];
  if (result != -1) ++playout[result];
}

void GameTree::Node::selection(int &row, int &col)
{
  bool whoTurn = board->getWhoTurn();

  double max = -1;

  int scoreSum = board->getScoreSum();
  int same = 1;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
    {
      // still need to adjust weight
      double value = board->getScore(r, c, whoTurn) + getUCBValue(r, c, whoTurn);

      if (value > max)
      {
        same = 1;

        max = value;
        row = r; col = c;
      }
      else if (value == max)
      {
        ++same;
        if (((double)rand() / RAND_MAX) <= (1. / same))
        {
          row = r; col = c;
        }
      }
    }
}

int GameTree::Node::simulation(int maxDepth)
{
  VirtualBoard* simulationBoard = new VirtualBoard(board);
  /* simulate until reach max depth */
  for (int d = 0; d < maxDepth; ++d)
  {
    int r, c; 
    simulationBoard->getHSP(r, c);

    /* if win, return who win */
    if (simulationBoard->play(r, c))
      return simulationBoard->getWhoTurn();
  }
  return -1;
}

double GameTree::Node::getUCBValue(int r, int c, bool color)
{
  return childNode[r][c]->getWinRate(color) + 
    sqrt(2 * log(childNode[r][c]->getTotalPlayout()) / playout[2]);
}