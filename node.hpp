#ifndef GAME_TREE_NODE_H
#define GAME_TREE_NODE_H

class GameTree::Node {
 public:
  /* consturctor for root */
  Node();
  /* constructor for node (WITHOUT root node) */
  Node(Node *parentNode, int row, int col);

  ~Node();

  /* result: -1 = tie, 0 = black win, 1 = white win */
  void update(int result);

  /* MCTS function, call by GameTree::selection
   * select child according to UCBValue and point's score
   * return false if every point is not empty */
  bool selection(int &row, int &col);

  /* MCTS function, call by GameTree::simulation
   * simulate the game at most maxDepth move,
   * and return who win (black = 0, white = 1, tie = -1) */
  int simulation(int maxDepth);

  /* get the Upper Confidence Bound value form child node */
  double getUCBValue(int r, int c, bool color);

  /* get some color's playout / total plaout */
  double getWinRate(bool color) {
    return ((playout[color] + (playout[2] - playout[color] - playout[(!color)]) / 2) /
            (double)playout[2]);
  }

  int getTotalPlayout() { return playout[2]; }

  Node* getParent() { return parent; }

  bool getIsChildWinning() { return isChildWinning; }

  bool getIsSelfWinning() { return isSelfWinning; }

  bool getWhoTurn() { return board->getWhoTurn(); }

  void clearPlayout() { playout[0] = 0; playout[1] = 0; playout[2] = 0;}

  Node *childNode[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
 private:
  /* 0 = black, 1 = white, 2 = total */
  int playout[3];
  bool isChildWinning, isSelfWinning;
  int winningChildRow, winningChildCol;
  VirtualBoard* board;
  Node *parent;
};

#endif