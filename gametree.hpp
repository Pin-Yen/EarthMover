class GameTree {
 public:
  static const int CHESSBOARD_DIMEN = 15;

  GameTree();

  /* Monty Carlo Tree Search 
   * will return the best point's position */
  void MCTS(int &row, int &col, int maxCycle);

  /* called when a REAL point is played, updates the currentRoot */
  void play(int row, int col);
 private:
  /* nested class */
  class Node;

  /* MCTS function
   * keep select the child node form the current node, until reach the leaf, 
   * when reach the leaf, will expand and return it */
  Node* selection();

  /* MCTS function
   * simulate the game at most maxDepth move, 
   * and return who win (black = 0, white = 1, tie = -1) */
  int simulation(Node* node, int maxDepth);

  /* MCTS function
   * back propagation form node, until reach the current node */
  void backProp(Node* node, int result);

  Node *root, *currentNode;
};