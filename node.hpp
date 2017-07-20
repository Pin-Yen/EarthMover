#ifndef GAME_TREE_NODE_H
#define GAME_TREE_NODE_H

#include <map>

class GameTree::Node {
 public:
  /* consturctor for root */
  Node();
  /* constructor for node (EXCEPT root node) */
  Node(Node *parentNode, int index, int parentWinOrLose);

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

  /* get winRate
   * NOTE: the win rate is for the upper layer(parent node)
   * and, in normal circumstances, only the upper layer will call this function*/
  double winRate() const {
    return ((playout_[0] + (playout_[2] - playout_[0] - playout_[1]) / 2.0) / (double)playout_[2]);
  }

  int totalPlayout() const { return playout_[2]; }

  Node* parent() const { return parent_; }

  bool winning() const { return winning_; }

  bool losing() const { return losing_; }

  void clearPlayout() { playout_[0] = 0; playout_[1] = 0; playout_[2] = 0; }

  void clearWinLose() { winning_ = false; losing_ = false; }

  int index() const { return index_; }

  Node* child(int index) const;

  Node* newChild(int index, int parentWinOrLose);

  class Iterator {
   public:
    Iterator(Node* node) :node_(node) {}

    bool operator!=(const Iterator& iter) const { return node_ != NULL; };

    Node* operator*() const { return node_; };

    const Iterator& operator++() { node_ = node_->next_; };
   private:
    Node* node_;
  };

  Iterator begin() const { return Iterator(child_); }

  Iterator end() const { return Iterator(NULL); }

 private:
  /* get the Upper Confidence Bound value form child node */
  double getUCBValue(const Node* node) const;

  Node *parent_, *child_, *next_;

  int index_;

  /* 0 = win, 1 = lose, 2 = total */
  int playout_[3];

  /* represent is current player winning or losing */
  bool winning_, losing_;
};

#endif