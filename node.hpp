#ifndef GAME_TREE_NODE_H
#define GAME_TREE_NODE_H

#include <map>

class GameTree::Node {
 public:
  /* consturctor for root */
  Node();
  /* constructor for node (EXCEPT root node) */
  Node(Node *parentNode, int parentWinOrLose);

  ~Node();

  static void *operator new(size_t size) {
    return pool.allocate(size);
  }

  static void operator delete(void *ptr, size_t size) { }

  /* update when tie */
  void update() { ++playout_[2]; }
  /* update, result: 0 -> win, 1 -> lose */
  void update(bool result) { ++playout_[2]; ++playout_[result]; }

  /* MCTS function, call by GameTree::selection
   * select child according to UCBValue and point's score
   * return -2 if select to leaf and no result yet, 0 for losing, 1 for winning, -1 if chessboard is full */
  // FIX: discription of return value not complete.
  int selection(int* index, VirtualBoard* board);

  /* get the Upper Confidence Bound value form child node */
  double getUCBValue(int index) const;

  /* get winRate
   * NOTE: the win rate is for the upper layer(parent node)
   * and, in normal circumstances, only the upper layer will call this function*/
  double winRate() const {
    return ((playout_[0] + (playout_[2] - playout_[0] - playout_[1]) / 2.0) / (double)playout_[2]);
  }

  int totalPlayout() const { return playout_[2]; }

  //int winPlayout() const {return playout_[0]; }

  //int losePlayout() const {return playout_[1]; }

  Node* parent() const { return parent_; }

  bool winning() const { return winning_; }

  bool losing() const { return losing_; }

  void clearPlayout() { playout_[0] = 0; playout_[1] = 0; playout_[2] = 0; }

  void clearWinLose() { winning_ = false; losing_ = false; }

  /* +1 for pass */
  Node* child(int index) const {
    auto child = child_.find(index);
    return child == child_.end() ? NULL : child->second;
  }

  Node* newChild(int index, int parentWinOrLose) {
    Node* node = new Node(this, parentWinOrLose);
    child_.insert(std::pair<int, Node*>(index, node));
    return node;
  }

 private:
  std::map<int, Node*> child_;

  /* 0 = win, 1 = lose, 2 = total */
  int playout_[3];

  /* represent is current player winning or losing */
  bool winning_, losing_;

  Node *parent_;
};

#endif