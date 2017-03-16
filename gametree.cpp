#include "gametree.hpp"
#include "node.hpp"

GameTree::GameTree()
{
  /* create the grand root(e.g. a root representing a blank board) */
  root = new Node();
}

void GameTree::selection(Node* node)
{
  while(true)
  {
    int r, c;
    //TODO: get highset UCB point and write into r, c;

    /* check if reach leaf*/

    bool reachLeaf = (node->childNode[r][c] == NULL);

    if (reachLeaf)
      node->childNode[r][c] = new Node(node, r, c);
    
    node = node->childNode[r][c];

    if (reachLeaf) return;
  }
}

int GameTree::simulation(int maxDepth, VirtualBoard* board)
{
  VirtualBoard* simulationBoard = new VirtualBoard(board);

  for (int d = 0; d < maxDepth; ++d)
  {
    int r, c; 
    simulationBoard->getHSP(r, c);

    if (simulationBoard->play(r, c))
      return simulationBoard->getWhoTurn();
  }
  return -1;
}

void GameTree::backProp(Node* node, int result)
{
  while(node != currentNode)
  {
    node->update(result);
    node = node->getParent();
  }
}

/* called when a REAL point is played, updates the currentRoot */
void GameTree::play(int row, int col)
{
  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col]  = new Node(currentNode, row, col);
  
  currentNode = currentNode->childNode[row][col] ;
}