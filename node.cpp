#include "node.h"

#include <cmath>
#include <iostream>

#include "gametree.h"
#include "gomoku/chesstype.h"
#include "gomoku/status.h"
#include "virtualboard.h"

#ifdef DEBUG
#include "objectcounter.h"
#endif

GameTree::Node::Node()
    : index_(-1),
      parent_(NULL),
      child_(NULL),
      next_(NULL),
      winOrLose_(0),
      totalPlayout_(0),
      winPlayout_(0),
      losePlayout_(0) {}

GameTree::Node::Node(Node *parentNode, int index, int parentWinOrLose)
    : index_(index),
      parent_(parentNode),
      child_(NULL),
      next_(NULL),
      winOrLose_(-parentWinOrLose),
      totalPlayout_(0),
      winPlayout_(0),
      losePlayout_(0) {

  // if losing, set parent to winning
  if (losing()) parent_->setWinning();
}

GameTree::Node::Node(Node *parentNode, const Node *source)
    : index_(source->index_),
      parent_(parentNode),
      child_(NULL),
      next_(NULL),
      winOrLose_(source->winOrLose_),
      totalPlayout_(source->totalPlayout_),
      winPlayout_(source->winPlayout_),
      losePlayout_(source->losePlayout_) {}

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

std::pair<GameTree::SearchStatus, GameTree::Node*> GameTree::Node::selection(
  VirtualBoard* board, MemoryPool* pool) {
  if (winning()) return std::make_pair(LOSE, this);
  if (losing()) return std::make_pair(WIN, this);

  // current max value
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();

  // checked index
  bool checked[225] = {0};

  Node* node;

  for (Node* childNode : *this) {
    int i = childNode->index();
    checked[i] = true;

    // If child node is winning then DO NOT select this point.
    if (childNode->winning()) {
      childWinning = true;
      continue;
    }

    // If there exists a point that wins in all previous simulations,
    // then select this point.
    if (childNode->winRate() == 1) {
      board->play(i);
      return std::make_pair(UNKNOWN, childNode);
    }

    double value = static_cast<double>(board->getScore(i)) / scoreSum +
                   getUCBValue(childNode);

    if (value > max) {
      max = value;
      node = childNode;
    }
  }

  int notCheckedIndex = board->getHSI(checked);

  if (notCheckedIndex != -1) {
    double value =
        static_cast<double>(board->getScore(notCheckedIndex)) / scoreSum +
        getUCBValue(NULL);

    if (value > max) {
      bool parentWinning = board->play(notCheckedIndex);
      node = newChild(notCheckedIndex, parentWinning, pool);

      if (parentWinning) {
        return std::make_pair(WIN, node);
      } else {
        return std::make_pair(LEAF, node);
      }
    }
  }

  // if no point can select
  if (max == -1) {
    // if every child wins, mark this point as a losing point
    if (childWinning) {
      setLosing();
      parent_->setWinning();

      return std::make_pair(WIN, this);
    }

    // if no useful point
    return std::make_pair(TIE, this);
  }

  board->play(node->index());
  return std::make_pair(UNKNOWN, node);
}

GameTree::Node* GameTree::Node::child(int index) const {
  for (Node* node : *this)
    if (node->index_ == index) return node;

  return NULL;
}

GameTree::Node* GameTree::Node::newChild(
    int index, int parentWinOrLose, MemoryPool* pool) {
  Node* node = new(pool) Node(this, index, parentWinOrLose);
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
  totalPlayout_ -= node->totalPlayout_;
  winPlayout_ -= node->winPlayout_;
  losePlayout_ -= node->losePlayout_;
}

void GameTree::Node::merge(const Node* node) {
  totalPlayout_ += node->totalPlayout_;
  winPlayout_ += node->winPlayout_;
  losePlayout_ += node->losePlayout_;

  if (notWinOrLose() && !node->notWinOrLose()) {
    winOrLose_ = node->winOrLose_;
  }
}

double GameTree::Node::getUCBValue(const Node* node) const {
  if (totalPlayout_ == 0) return 0;

  if (node != NULL) {
    return (node->winRate() +
            sqrt(.5 * log(totalPlayout_) / (1 + node->totalPlayout())));
  } else {
    return (sqrt(.5 * log(totalPlayout_) / 1));
  }
}
