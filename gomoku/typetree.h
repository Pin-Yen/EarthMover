#ifndef GOMOKU_TYPETREE_H_
#define GOMOKU_TYPETREE_H_

#include <iomanip>
#include <iostream>

#include "chesstype.h"
#include "status.h"
#include "virtualboardgomoku.h"

#ifdef DEBUG
#include "../objectcounter.h"
#endif

template <int StatusLength>
template <class DerivedTypeTree>
class VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree {
 public:
  // Given a status array, classify its chesstype and return it
  static ChessType classify(const STATUS *status);

  static void init();

 protected:
  struct Node {
    // Next point occupied by:
    // 0: black, 1: white, 2:empty 3: bound
    Node *childNode[4];

    ChessType type;

    bool jump, leaf;

    Node(): jump(false), leaf(false) {
      for (int i = 0; i < 4; ++i)
        childNode[i] = NULL;

      #ifdef DEBUG
      ObjectCounter::registerTypeTreeNode();
      #endif
    }

    ~Node() {
      for (int i = 0; i < 4; ++i)
        if (childNode[i] != NULL) {
          delete childNode[i];
          childNode[i] = NULL;
        }
      #ifdef DEBUG
      ObjectCounter::unregisterTypeTreeNode();
      #endif
    }
  };

  static const int ANALYZE_LENGTH = StatusLength + 1;

 private:
  // cut the tree node that all child has same result
  static ChessType* cutSameResultChild(Node *root);
};

template <int StatusLength>
template <class DerivedTypeTree>
void VirtualBoardGomoku<StatusLength>::Evaluator::
    TypeTree<DerivedTypeTree>::init() {
  // check if tree exists
  if (DerivedTypeTree::isInit) return;
  DerivedTypeTree::isInit = true;

  DerivedTypeTree::root = new Node();

  DerivedTypeTree::plantTree();

  DerivedTypeTree::cutSameResultChild(DerivedTypeTree::root);
}

template <int StatusLength>
template <class DerivedTypeTree>
ChessType VirtualBoardGomoku<StatusLength>::Evaluator::
    TypeTree<DerivedTypeTree>::classify(const STATUS *status) {
  Node* node = DerivedTypeTree::root;

  for (int move = -1, start = StatusLength / 2 - 1; ; move = 1, ++start)
    for (int checkPoint = start; ; checkPoint += move) {
      // according to the status to enter the child node
      node = node->childNode[status[checkPoint]];

      // if reach leaf, return type
      if (node->leaf) {
        return ChessType(node->type);
      }

      // if reach different color, change direction
      if (node->jump)
        break;
    }
}

template <int StatusLength>
template <class DerivedTypeTree>
ChessType* VirtualBoardGomoku<StatusLength>::Evaluator
    ::TypeTree<DerivedTypeTree>::cutSameResultChild(Node *node) {
  if (node->leaf)
    return &(node->type);

  ChessType *currentType = NULL;
  bool canCut = true;

  for (int i = 0; i < 4; ++i)
    if (node->childNode[i] != NULL) {
      // if this child node is not NULL, recursion and get return
      ChessType* returnType = cutSameResultChild(node->childNode[i]);

      if (returnType == NULL)
        // if children cannot be cut, then this node also cannot be cut
        canCut = false;
      else if (currentType == NULL)
        currentType = returnType;
      else if (*currentType != *returnType)
        // if different child has different result, cannot cut this node
        canCut = false;
    }

  if (!canCut) return NULL;
  // cut this node, free all children and set this node's type
  node->type = ChessType(*currentType);

  node->leaf = true;

  currentType = &(node->type);

  for (int i = 0; i < 4; ++i)
    if (node->childNode[i] != NULL) {
      delete node->childNode[i];
      node->childNode[i] = NULL;
    }

  return currentType;
}

#endif
