#include "gametree.hpp"
#include "node.hpp"

GameTree::GameTree()
{
  /* create the grand root(e.g. a root representing a blank board) */
  root = new Node();
}

void GameTree::MCTS(int &row, int &col, int maxCycle)
{
  const int SIMULATE_DEPTH = 50;
  for (int cycle = 0; cycle < maxCycle; ++cycle)
  {
    Node* node = selection();
    int result = simulation(node, SIMULATE_DEPTH);
    backProp(node, result);
  }
  //TODO: return the point that select most times
}

GameTree::Node* GameTree::selection()
{
  Node* node = currentNode;

  bool reachLeaf = false;
  while(!reachLeaf)
  {
    int r, c;
    node->selection(r, c);

    /* check if reach leaf*/
    reachLeaf = (node->childNode[r][c] == NULL);

    if (reachLeaf)
      node->childNode[r][c] = new Node(node, r, c);
    
    node = node->childNode[r][c];
  }

  return node;
}

int GameTree::simulation(Node* node, int maxDepth)
{
  return node->simulation(maxDepth);
}

void GameTree::backProp(Node* node, int result)
{
  while(node != currentNode)
  {
    node->update(result);
    node = node->getParent();
  }
}

void GameTree::play(int row, int col)
{
  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col]  = new Node(currentNode, row, col);
  
  currentNode = currentNode->childNode[row][col] ;
}