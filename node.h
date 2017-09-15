#ifndef NODE_H_
#define NODE_H_

#include <stdint.h>
#include <map>

#include "const.h"
#include "gametree.h"

class GameTree::Node {
 public:
  // consturctor for root
  Node();
  // constructor for node (EXCEPT root node)
  Node(Node *parent, int index, GameStatus parentStatus);

  // Constructor for Copy data from another node.
  Node(Node *parent, const Node *source);

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

  // MCTS function, call by GameTree::selection.
  // Select child according to UCBValue and point's score.
  // Return selected node and status.
  std::pair<SearchStatus, GameTree::Node*> selection(
      VirtualBoard* board, MemoryPool* pool);

  // Update playout.
  void update(SearchStatus result) {
    ++count_;
    if (result == WIN) ++winLoseCount_;
    if (result == LOSE) --winLoseCount_;
  }

  // Get win rate.
  // NOTE: The win rate is for the upper layer(parent node).
  double winRate() const { return (count_ + winLoseCount_) / (count_ * 2.0); }

  // Delete all children (or except for the 'exceptNode').
  void deleteChildren(MemoryPool* pool);
  void deleteChildrenExcept(Node* exceptNode, MemoryPool* pool);

  // Clear count and winLose.
  void clear() {
    count_ = 0;
    winLoseCount_ = 0;
    gameStatus_ = NOTHING;
    //winOrLose_ = 0;
  }

  bool hasChild() const { return child_ != NULL; }

  // Getter.
  Node* parent() const { return parent_; }
  int count() const { return count_; }
  int index() const { return index_; }
  GameStatus gameStatus() const { return gameStatus_; }
  bool winning() const { return gameStatus_ == WINNING; }
  bool losing() const { return gameStatus_ == LOSING; }
  bool notWinOrLose() const { return gameStatus_ == NOTHING; }

  void setWinning() { gameStatus_ = WINNING; }
  void setLosing() { gameStatus_ = LOSING; }

  // Get child node with specify index.
  Node* child(int index) const;

  // Append a new child and return it, parameters for node's constructor.
  Node* newChild(int index, GameStatus parentStatus, MemoryPool* pool);
  Node* newChild(Node* source, MemoryPool* pool);

  // Custom iterator, iterates over node's child.
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
  // Get the Upper Confidence Bound value from child node.
  double getUCBValue(const Node* node) const;

  // parent, child, next node.
  Node *parent_, *child_, *next_;

  int count_, winLoseCount_;
  int index_;

  GameStatus gameStatus_;
};

#endif  // NODE_H_
