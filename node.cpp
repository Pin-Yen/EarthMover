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

GameTree::Node::Node() {
  index_ = -1;

  clearPlayout();
  clearWinLose();

  /* initiaize parent node */
  parent_ = NULL;
}

GameTree::Node::Node(Node *parentNode, int index, int parentWinOrLose) {
  index_ = index;

  clearPlayout();

  /* initiaize parent node */
  parent_ = parentNode;

  switch (parentWinOrLose) {
    case 1:
      winning_ = false;
      losing_ = true;
      break;
    case -1:
      winning_ = true;
      losing_ = false;
      break;
    default:
      clearWinLose();
  }

  /* if is losing, set parent_ to winning */
  if (losing_)
    parent_->winning_ = true;
}

int GameTree::Node::selection(int* index, VirtualBoard* board) {
  if (winning_) return 1;
  if (losing_) return 0;

  /* current max value */
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();
  int same = 1;

  bool checked[CHILD_LENGTH] = {0};

  for (Node* childNode : *this) {
    int i = childNode->index();
    checked[i] = true;

    /* if child node is winning then DO NOT select this point */
    if (childNode->winning_) {
      childWinning = true;
      continue;
    }

    /* if there exists a point that wins in all previous simulations, select this point */
    if (childNode->winRate() == 1) {
      *index = i;
      return -2;
    }

    int score = board->getScore(i);

    double ucbValue = getUCBValue(childNode);

    double value = ((double)score / scoreSum) + ucbValue;

    if (value > max) {
      same = 1;

      max = value;
      *index = i;
    } else if (value == max) {
      ++same;
      if (((double)rand() / RAND_MAX) <= (1. / same)) {
        *index = i;
      }
    }
  }

  double ucbValue = getUCBValue(NULL);

  for (int i = 0; i < CHILD_LENGTH; ++i) {
    if (checked[i]) continue;

    int score = board->getScore(i);
    /* skip if this point is occupied */
    if (score == -1) continue;

    double value = ((double)score / scoreSum) + ucbValue;

    if (value > max) {
      same = 1;

      max = value;
      *index = i;
    } else if (value == max) {
      ++same;
      if (((double)rand() / RAND_MAX) <= (1. / same)) {
        *index = i;
      }
    }
  }

  /* if no point can select */
  if (max == -1) {
    /* if every child wins, mark this point as a losing point */
    if (childWinning) {
      losing_ = true;
      parent_->winning_ = true;

      return 0;
    }

    /* if the chessboard is FULL */
    return -1;
  }

  return -2;
}

GameTree::Node* GameTree::Node::child(int index) const {
  Node* node = child_;
  for (; node != NULL; node = node->next_) {
    if (node->index_ == index) break;
  }
  return node;
}

GameTree::Node* GameTree::Node::newChild(int index, int parentWinOrLose) {
  Node* node = new Node(this, index, parentWinOrLose);
  node->next_ = child_;
  child_ = node;
  return node;
}

double GameTree::Node::getUCBValue(const Node* node) const {
  if (playout_[2] == 0)
    return 0;

  if (node != NULL) {
    return (node->winRate() +
            sqrt(0.5 * log10(playout_[2]) / (1 + node->totalPlayout())));
  } else {
    return (sqrt(0.5 * log10(playout_[2]) / 1));
  }
}