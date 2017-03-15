#include "gametree.hpp"

GameTree::GameTree()
{
  /* create the grand root(e.g. a root representing a blank board) */
  root = new GameNode();
}

void GameTree::selection(GameNode* node)
{
  while(true)
  {
    int r, c;
    //TODO: get highset UCB point and write into r, c;

    /* check if reach leaf*/
    bool reachLeaf = (node->getChildNode(r, c) == NULL);

    if (reachLeaf)
      node->getChildNode(r, c) = new GameNode(node, r, c);
    
    node = node->getChildNode(r, c);

    if (reachLeaf) return;
  }
}

int GameTree::simulation(int maxDepth, VirtualBoard* board)
{
  for (int d = 0; d < maxDepth; ++d)
  {
    int r, c; board->getHSP(r, c);

    if (board->play(r, c))
      return board->getWhoTurn();
  }
  return -1;
}

void backProp(GameNode* node, int result)
{
  while(node != currentNode)
  {
    node->update(int result)
    node = node->getParent();
  }
}

/* called when a REAL point is played, updates the currentRoot */
void GameTree::play(int row, int col)
{
  currentNode = currentNode->getChildNode(row, col);
}