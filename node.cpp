#include "gomoku/chesstype.hpp"
#include "gomoku/status.hpp"
#include "virtualboard.hpp"
#include "gametree.hpp"
#include "node.hpp"

#include <iostream>
#include <cmath>

#ifdef DEBUG
#include "objectcounter.hpp"
#endif

GameTree::Node::Node() : index_(-1), parent_(NULL), winOrLose_(0), playout_{0} {}

GameTree::Node::Node(Node *parentNode, int index, int parentWinOrLose)
    : index_(index),
      parent_(parentNode),
      winOrLose_(-parentWinOrLose),
      playout_{0} {

  // if losing, set parent to winning
  if (losing()) parent_->setWinning();
}

int GameTree::Node::selection(int* index, VirtualBoard* board) {
  if (winning()) return 1;
  if (losing()) return 0;

  // current max value
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();

  // checked index
  bool checked[CHILD_LENGTH] = {0};

  for (Node* childNode : *this) {
    int i = childNode->index();
    checked[i] = true;

    // if child node is winning then DO NOT select this point
    if (childNode->winning()) {
      childWinning = true;
      continue;
    }

    // if there exists a point that wins in all previous simulations, select this point
    if (childNode->winRate() == 1) {
      *index = i;
      return -2;
    }

    double value = ((double)board->getScore(i) / scoreSum) + getUCBValue(childNode);

    if (value > max) {
      max = value;
      *index = i;
    }
  }

  int notCheckedIndex = board->getHSI(checked);

  if (notCheckedIndex != -1) {
    double value = ((double)board->getScore(notCheckedIndex) / scoreSum) + getUCBValue(NULL);

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

double GameTree::Node::getUCBValue(const Node* node) const {
  if (playout_[2] == 0) return 0;

  if (node != NULL) {
    return (node->winRate() +
            sqrt(0.5 * log10(playout_[2]) / (1 + node->totalPlayout())));
  } else {
    return (sqrt(0.5 * log10(playout_[2]) / 1));
  }
}