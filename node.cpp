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

GameTree::Node::Node() : index_(-1), parent_(NULL), child_(NULL), next_(NULL),
                         winOrLose_(0), playout_{0} {}

GameTree::Node::Node(Node *parentNode, int index, int parentWinOrLose)
    : index_(index),
      parent_(parentNode),
      child_(NULL),
      next_(NULL),
      winOrLose_(-parentWinOrLose),
      playout_{0} {

  // if losing, set parent to winning
  if (losing()) parent_->setWinning();
}

GameTree::Node::Node(Node *parentNode, const Node *source)
    : index_(source->index_),
      parent_(parentNode),
      child_(NULL),
      next_(NULL),
      winOrLose_(source->winOrLose_) {
  for (int i = 0; i < 3; ++i) {
    playout_[i] = source->playout_[i];
  }
}

void GameTree::Node::deleteChildren() {
  Node *child = child_, *next;

  // recursive delete all child
  // the next node should be saved before current node is deleted
  while (child != NULL) {
    next = child->next_;
    child->deleteChildren();
    delete child;
    child = next;
  }

  child_ = NULL;
}

void GameTree::Node::deleteChildrenExcept(Node* exceptNode) {
  Node *child = child_, *next;

  // recursive delete all child except "exceptNode"
  // the next node should be saved before current node is deleted
  while (child != NULL) {
    if (child == exceptNode) {
      child = child->next_;
    } else {
      next = child->next_;
      child->deleteChildren();
      delete child;
      child = next;
    }
  }

  child_ = exceptNode;
  exceptNode->next_ = NULL;
}

int GameTree::Node::selection(int* index, VirtualBoard* board) {
  if (winning()) return 1;
  if (losing()) return 0;

  // current max value
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();

  // checked index
  bool checked[225] = {0};

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
      *index = i;
      return -2;
    }

    double value = static_cast<double>(board->getScore(i)) / scoreSum +
                   getUCBValue(childNode);

    if (value > max) {
      max = value;
      *index = i;
    }
  }

  int notCheckedIndex = board->getHSI(checked);

  if (notCheckedIndex != -1) {
    double value =
        static_cast<double>(board->getScore(notCheckedIndex)) / scoreSum +
        getUCBValue(NULL);

    if (value > max) {
      max = value;
      *index = notCheckedIndex;
    }
  }

  // if no point can select
  if (max == -1) {
    // if every child wins, mark this point as a losing point
    if (childWinning) {
      setLosing();
      parent_->setWinning();

      return 0;
    }

    // if no useful point
    return -1;
  }

  return -2;
}

GameTree::Node* GameTree::Node::child(int index) const {
  for (Node* node : *this)
    if (node->index_ == index) return node;

  return NULL;
}

GameTree::Node* GameTree::Node::newChild(int index, int parentWinOrLose) {
  Node* node = new Node(this, index, parentWinOrLose);
  // append node to first
  node->next_ = child_;
  child_ = node;
  return node;
}

GameTree::Node* GameTree::Node::newChild(Node* source) {
  Node* node = new Node(this, source);
  // append node to first
  node->next_ = child_;
  child_ = node;
  return node;
}

void GameTree::Node::minus(const Node* node) {
  for (int i = 0; i < 3; ++i) {
    playout_[i] -= node->playout_[i];
  }
}

void GameTree::Node::merge(const Node* node) {
  for (int i = 0; i < 3; ++i) {
    playout_[i] += node->playout_[i];
  }

  if (notWinOrLose() && !node->notWinOrLose()) {
    winOrLose_ = node->winOrLose_;
  }
}

double GameTree::Node::getUCBValue(const Node* node) const {
  if (playout_[2] == 0) return 0;

  if (node != NULL) {
    return (node->winRate() +
            sqrt(.5 * log(playout_[2]) / (1 + node->totalPlayout())));
  } else {
    return (sqrt(.5 * log(playout_[2]) / 1));
  }
}
