#include "gametree.hpp"

GameTree::GameTree()
{
  /* create the grand root(e.g. a root representing a blank board) */
  root = new GameNode(NULL, NULL, -1, -1);

}

/* plays a point in the simulation process,
 * virtualPlay() should be managed by some function in gametree */
void GameTree::virtualPlay(int depth, Node *currentNode){
  
  /* if the current node has results, backprop. */
  if(currentNode->hasResult()){
    currentNode->updateAndBackprop()
    return;
  }
  /* if current node has no results, play at a random point */
  else{
    currentNode = currentNode->pickRandomPoint();
    virtualPlay(depth + 1, currentNode);
  }
}

/* called when a REAL point is played, updates the currentRoot */
void GameTree::play(int row, int col)
{
  currentRoot = currentRoot->getChildNode(row, col);
}