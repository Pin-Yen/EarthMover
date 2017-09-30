#include "node.h"

#include <cmath>
#include <iostream>

#include "const.h"
#include "gametree.h"
#include "gomoku/chesstype.h"
#include "virtualboard.h"

GameTree::Node::Node()
    : index_(-1),
      parent_(NULL),
      child_(NULL),
      next_(NULL),
      gameStatus_(NOTHING),
      count_(0),
      //winLoseCount_(0),
      score_(0) {}

GameTree::Node::Node(Node *parent, int index, GameStatus parentStatus)
    : index_(index),
      parent_(parent),
      child_(NULL),
      next_(NULL),
      gameStatus_(static_cast<GameStatus>(-static_cast<int>(parentStatus))),
      count_(0),
      //winLoseCount_(0),
      score_(0) {

  // if losing, set parent to winning
  if (losing()) parent_->setWinning();
}

GameTree::Node::Node(Node *parent, const Node *source)
    : index_(source->index_),
      parent_(parent),
      child_(NULL),
      next_(NULL),
      gameStatus_(source->gameStatus_),
      count_(source->count_),
      //winLoseCount_(source->winLoseCount_),
      score_(source->score_) {}

void GameTree::Node::deleteChildren(MemoryPool* pool) {
  Node *child = child_, *next;

  // recursive delete all child
  // the next node should be saved before current node is deleted
  while (child != NULL) {
    next = child->next_;
    child->deleteChildren(pool);
    operator delete(child, pool);
    child = next;
  }

  child_ = NULL;
}

void GameTree::Node::deleteChildrenExcept(Node* exceptNode, MemoryPool* pool) {
  Node *child = child_, *next;

  // recursive delete all child except "exceptNode"
  // the next node should be saved before current node is deleted
  while (child != NULL) {
    if (child == exceptNode) {
      child = child->next_;
    } else {
      next = child->next_;
      child->deleteChildren(pool);
      operator delete(child, pool);
      child = next;
    }
  }

  child_ = exceptNode;
  exceptNode->next_ = NULL;
}

std::pair<SearchStatus, GameTree::Node*> GameTree::Node::selection(
  VirtualBoard* board, MemoryPool* pool) {
  if (winning()) return std::make_pair(LOSE, this);  // Parent lose.
  if (losing()) return std::make_pair(WIN, this);    // Parent Win.

  // current max value
  double max = -1;

  bool childWinning = false;

  double scoreSum = board->getScoreSum();

  // checked index
  bool checked[board->length()] = {0};

  Node* node;

  for (Node* child : *this) {
    int i = child->index();
    checked[i] = true;

    // If child node is winning then DO NOT select this point.
    if (child->winning()) {
      childWinning = true;
      continue;
    }

    // If there exists a point that wins in all previous simulations,
    // then select this point.
    //if (child->winRate() == 1) {
    //  board->play(i);
    //  return std::make_pair(UNKNOWN, child);
    //}

    double val = board->getScore(i) / scoreSum + getUCBValue(child);

    if (val > max) {
      max = val;
      node = child;
    }
  }

  int uncheckIndex = board->getHSI(checked);

  if (uncheckIndex != -1) {
    double val = board->getScore(uncheckIndex) / scoreSum + getUCBValue(NULL);

    if (val > max) {
      GameStatus status = board->play(uncheckIndex);
      node = newChild(uncheckIndex, status, pool);

      return status == WINNING ? std::make_pair(LOSE, this) :  // Parent lose.
                                 std::make_pair(LEAF, node);   // Leaf node.
    }
  }

  // If no point selected.
  if (max == -1) {
    // If every child wins, this point is a losing point.
    if (childWinning) {
      setLosing();
      parent_->setWinning();

      return std::make_pair(WIN, this);  // Parent win.
    }

    // If no useful point, return tie.
    return std::make_pair(TIE, this);
  }

  // Return selected node.
  board->play(node->index());
  return std::make_pair(UNKNOWN, node);
}

GameTree::Node* GameTree::Node::child(int index) const {
  for (Node* node : *this)
    if (node->index_ == index) return node;

  return NULL;
}

GameTree::Node* GameTree::Node::newChild(
    int index, GameStatus parentStatus, MemoryPool* pool) {
  Node* node = new(pool) Node(this, index, parentStatus);
  // append node to first
  node->next_ = child_;
  child_ = node;
  return node;
}

GameTree::Node* GameTree::Node::newChild(Node* source, MemoryPool* pool) {
  Node* node = new(pool) Node(this, source);
  // append node to first
  node->next_ = child_;
  child_ = node;
  return node;
}

void GameTree::Node::minus(const Node* node) {
  count_ -= node->count_;
  //winLoseCount_ -= node->winLoseCount_;
  score_ -= node->score_;
}

void GameTree::Node::merge(const Node* node) {
  count_ += node->count_;
  //winLoseCount_ += node->winLoseCount_;
  score_ += node->score_;

  if (notWinOrLose() && !node->notWinOrLose()) {
    gameStatus_ = node->gameStatus_;
  }
}

double GameTree::Node::getUCBValue(const Node* node) const {
  if (count_ == 0) return 0;

  if (node != NULL) {
    return (node->winRate() +
            sqrt(log(count_) / (1 + node->count_)));
  } else {
    return sqrt(log(count_));
  }
}
