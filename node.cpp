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
  /* initialize all childNodes to NULL */
  //for (int i = 0; i < CHILD_LENGTH + 1; ++i)
  //  childNode[i] = NULL;

  clearPlayout();
  clearWinLose();

  /* initiaize parent node */
  parent_ = NULL;

  #ifdef DEBUG
  ObjectCounter::registerNode();
  #endif
}

GameTree::Node::Node(Node *parentNode, int parentWinOrLose) {
  /* initialize all childNodes to NULL */
  //for (int i = 0; i < CHILD_LENGTH + 1; ++i)
  //  childNode[i] = NULL;

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

  #ifdef DEBUG
  ObjectCounter::registerNode();
  #endif
}

GameTree::Node::~Node() {
  /*
  for (int i = 0; i < CHILD_LENGTH + 1; ++i)
    if (childNode[i] != NULL)
      delete childNode[i];*/

  #ifdef DEBUG
  ObjectCounter::unregisterNode();
  #endif
}

int GameTree::Node::selection(int* index, VirtualBoard* board) {
  if (winning_) return 1;
  if (losing_) return 0;

  /* current max value */
  double max = -1;

  bool childWinning = false;

  int scoreSum = board->getScoreSum();
  int same = 1;

  for (int i = 0; i < CHILD_LENGTH; ++i) {
    int score = board->getScore(i);

    /* skip if this point is occupied */
    if (score == -1) continue;

    Node* childNode = child(i);

    if (childNode != NULL) {
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
      }

      double ucbValue = getUCBValue(i);

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

double GameTree::Node::getUCBValue(int index) const {
  if (playout_[2] == 0)
    return 0;

  const Node* childNode = child(index);
  if (childNode != NULL) {
    return (childNode->winRate() +
            sqrt(0.5 * log10(playout_[2]) / (1 + childNode->totalPlayout())));
  } else {
    return (sqrt(0.5 * log10(playout_[2]) / 1));
  }
}