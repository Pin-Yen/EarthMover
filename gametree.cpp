#include "gametree.hpp"
#include "node.hpp"

GameTree::GameTree() {
  /* create the grand root(e.g. a root representing a blank board) */
  root = new Node();
}

void GameTree::MCTS(int &row, int &col, int maxCycle) {
  const int SIMULATE_DEPTH = 50;

  for (int cycle = 0; cycle < maxCycle; ++cycle) {
    Node* node = selection();
    int result = simulation(node, SIMULATE_DEPTH);
    backProp(node, result);
  }

  // return the point that select most times
  int mostTimes = -1;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (currentNode->childNode[r][c] != NULL)
        if (currentNode->childNode[r][c]->getTotalPlayout > mostTimes) {
          // ?? should we pick a random point if there are multiple best points,
          // or just pick the first point ??
          row = r;
          col = c;
          mostTimes = currentNode->childNode[r][c]->getTotalPlayout;
        }
}

GameTree::Node* GameTree::selection() {
  Node* node = currentNode;

  while (true) {
    int r, c;
    node->selection(r, c);

    //TODO: handle if already win when play at child

    /* check if reach leaf */
    if (node->childNode[r][c] == NULL) {
      node->childNode[r][c] = new Node(node, r, c);
      return node->childNode[r][c];
    }
    
    node = node->childNode[r][c];
  }
}

int GameTree::simulation(Node* node, int maxDepth) {
  return node->simulation(maxDepth);
}

void GameTree::backProp(Node* node, int result) {
  while (node != currentNode) {
    node->update(result);
    node = node->getParent();
  }
}

void GameTree::play(int row, int col) {
  if (currentNode->childNode[row][col] == NULL)
    currentNode->childNode[row][col] = new Node(currentNode, row, col);
  
  currentNode = currentNode->childNode[row][col] ;
}