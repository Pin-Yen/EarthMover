#ifndef GAME_TREE_NODE_H
#define GAME_TREE_NODE_H

#include <stdint.h>

#include <map>

class GameTree::Node {
 public:
  // consturctor for root
  Node();
  // constructor for node (EXCEPT root node)
  Node(Node *parentNode, int index, int parentWinOrLose);

  // overload new and delete operator for memory pool
  static void *operator new(size_t size) { return pool_.allocate(size); }
  static void operator delete(void *ptr, size_t size) { pool_.deallocate(ptr, size); }

  // update when tie
  void update() { ++playout_[2]; }
  // update, result: 0 -> win, 1 -> lose
  void update(bool result) { ++playout_[2]; ++playout_[result]; }

  // MCTS function, call by GameTree::selection
  // select child according to UCBValue and point's score
  // return -2 if select to leaf and no result yet, 0 for losing, 1 for winning, -1 if chessboard is full
  // FIX: discription of return value not complete.
  int selection(int* index, VirtualBoard* board);

  // get winRate
  // NOTE: the win rate is for the upper layer(parent node)
  double winRate() const {
    return ((playout_[0] + (playout_[2] - playout_[0] - playout_[1]) / 2.0) / (double)playout_[2]);
  }

  // delete all children (or except for the "exceptNode")
  void deleteChildren();
  void deleteChildrenExcept(Node* exceptNode);

  // getter
  Node* parent() const { return parent_; }
  int totalPlayout() const { return playout_[2]; }
  int index() const { return index_; }
  int winOrLose() const { return winOrLose_; }
  bool winning() const { return winOrLose_ > 0; }
  bool losing() const { return winOrLose_ < 0; }
  bool notWinOrLose() const { return winOrLose_ == 0; }

  void setWinning() { winOrLose_ = 1; }
  void setLosing() { winOrLose_ = -1; }

  // get child node with specify index
  Node* child(int index) const;

  // append a new child and return it, parameters for node's constructor
  Node* newChild(int index, int parentWinOrLose);

  // custom iterator, iterates over node's child
  // Usage: for (Node* child : *node)
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
  // get the Upper Confidence Bound value form child node
  double getUCBValue(const Node* node) const;

  // parent, child, next node
  Node *parent_, *child_, *next_;

  // 0 = win, 1 = lose, 2 = total
  int playout_[3];

  int16_t index_;

  // represent is current player winning or losing
  // 1: winning, -1: losing, 0: nothing
  int8_t winOrLose_;
};

#endif
