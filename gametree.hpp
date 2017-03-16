#include "gomoku/freestyle/virtualboard.hpp"

class GameTree
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  GameTree();

  void play(int row, int col);
private:
  /* nested class */
  class Node;

  /* MCTS function
   * keep select the child node, until reach the leaf */
  void selection(Node* node);

  /* MCTS function
   * simulate the game at most maxDepth move, 
   * and return who win (black = 0, white = 1, tie = -1) */
  int simulation(int maxDepth, VirtualBoard* board);

  /* MCTS function
   * back propagation form node to current node */
  void backProp(Node* node, int result);

  Node *root, *currentNode;
};