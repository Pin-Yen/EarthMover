#ifndef NODE_H_
#define NODE_H_

#include <stdint.h>
#include <map>

#include "gametree.h"

class GameTree::Node {
 public:
  // consturctor for root
  Node();
  // constructor for node (EXCEPT root node)
  Node(Node *parentNode, int index, int parentWinOrLose);

  // Constructor for Copy data from another node.
  Node(Node *parentNode, const Node *source);

  // overload new and delete operator for memory pool
  static void *operator new(size_t) = delete;
  static void operator delete(void *ptr) = delete;

  static void *operator new(size_t /*size*/, MemoryPool* pool) {
    return pool->allocate();
  }
  static void operator delete(void *ptr, MemoryPool* pool) {
    pool->deallocate(ptr);
  }

  void minus(const Node* node);

  void merge(const Node* node);

  // Update playout.
  void update(SearchStatus result) {
    ++totalPlayout_;
    if (result == WIN) ++winPlayout_;
    if (result == LOSE) ++losePlayout_;
  }

  // MCTS function, call by GameTree::selection.
  // Select child according to UCBValue and point's score, and return status.
  std::pair<GameTree::SearchStatus, GameTree::Node*> selection(
      VirtualBoard* board, MemoryPool* pool);

  // get winRate
  // NOTE: the win rate is for the upper layer(parent node)
  double winRate() const {
    return ((winPlayout_ + (totalPlayout_ - winPlayout_ - losePlayout_) / 2.) /
            static_cast<double>(totalPlayout_));
  }

  // delete all children (or except for the "exceptNode")
  void deleteChildren(MemoryPool* pool);
  void deleteChildrenExcept(Node* exceptNode, MemoryPool* pool);

  // clear playout and winLose
  void clear() {
    totalPlayout_ = 0;
    winPlayout_ = 0;
    losePlayout_ = 0;
    winOrLose_ = 0;
  }

  bool hasChild() const { return child_ != NULL; }

  // getter
  Node* parent() const { return parent_; }
  int totalPlayout() const { return totalPlayout_; }
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
  Node* newChild(int index, int parentWinOrLose, MemoryPool* pool);

  Node* newChild(Node* source, MemoryPool* pool);

  // custom iterator, iterates over node's child
  // Usage: for (Node* child : *node)
  class Iterator {
   public:
    explicit Iterator(Node* node) : node_(node) {}

    bool operator!=(const Iterator& iter) const { return node_ != NULL; }

    Node* operator*() const { return node_; }

    const Iterator& operator++() { node_ = node_->next_; }

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

  int totalPlayout_, winPlayout_, losePlayout_;

  int16_t index_;

  // represent is current player winning or losing
  // 1: winning, -1: losing, 0: nothing
  int8_t winOrLose_;
};

#endif  // NODE_H_
