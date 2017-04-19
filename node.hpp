#ifndef GAME_TREE_NODE_H
#define GAME_TREE_NODE_H

class GameTree::Node {
 public:
  /* consturctor for root */
  Node();
  /* constructor for node (EXCEPT root node) */
  Node(Node *parentNode, int row, int col, bool losing);

  ~Node();

  /* update when tie */
  void update() { ++playout[2]; }
  /* update, result: 0 -> win, 1 -> lose */
  void update(bool result) { ++playout[2]; ++playout[result]; }

  /* MCTS function, call by GameTree::selection
   * select child according to UCBValue and point's score
   * return -2 if select to leaf and no result yet, 0 for losing, 1 for winning, -1 if chessboard is full */
  // FIX: discription of return value not complete.
  int selection(int &row, int &col, VirtualBoard* board);

  /* get the Upper Confidence Bound value form child node */
  double getUCBValue(int r, int c);

  /* get winRate
   * NOTE: the win rate is for the upper layer(parent node)
   * and, in normal circumstances, only the upper layer will call this function*/
  double winRate() {
    return ((playout[0] + (playout[2] - playout[0] - playout[1]) / 2.0) / (double)playout[2]);
  }

  int totalPlayout() { return playout[2]; }

  Node* getParent() { return parent; }

  bool winning() { return winning_; }

  bool losing() { return losing_; }

  void clearPlayout() { playout[0] = 0; playout[1] = 0; playout[2] = 0; }

  void clearWinLose() { winning_ = false; losing_ = false; }

  Node *childNode[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
 private:
  /* 0 = win, 1 = lose, 2 = total */
  int playout[3];

  /* represent is current player winning or losing */
  bool winning_, losing_;

  Node *parent;
};

#endif