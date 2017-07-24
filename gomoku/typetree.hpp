#ifndef TYPE_TREE_H
#define TYPE_TREE_H

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

template <int StatusLength>
template <class DerivedTypeTree>
class VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree {
 public:
  // Given a status array, classify its chesstype and returns black's type in type[0], white's type in type[1].
  static void classify(const STATUS *status, ChessType *(type[2]));

  static void init();

 protected:
  struct Node {
    // Next point occupied by:
    // 0: black, 1: white, 2:empty 3: bound
    Node *childNode[4];

    ChessType type[2];

    bool jump, leaf;

    Node(): jump(false), leaf(false) {
      for (int i = 0; i < 4; ++i)
        childNode[i] = NULL;

      #ifdef DEBUG
      ObjectCounter::registerTypeTreeNode();
      #endif
    }

    ~Node() {
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

#include "chesstype.hpp"
#include "status.hpp"
#include "../virtualboard.hpp"
#include "virtualboardgomoku.hpp"

#include <iostream>
#include <iomanip>

#ifdef DEBUG
#include "../objectcounter.hpp"
#endif

template <int StatusLength>
template <class DerivedTypeTree>
void VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree<DerivedTypeTree>::init() {
  // check if tree exists
  if (DerivedTypeTree::isInit) return;
  DerivedTypeTree::isInit = true;

  DerivedTypeTree::root = new Node();

  DerivedTypeTree::plantTree();

  DerivedTypeTree::cutSameResultChild(DerivedTypeTree::root);
}

template <int StatusLength>
template <class DerivedTypeTree>
void VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree<DerivedTypeTree>::classify(const STATUS *status, ChessType *(type[2])) {
  Node* node = DerivedTypeTree::root;

  for (int move = -1, start = StatusLength / 2 - 1; ; move = 1, ++start)
    for (int checkPoint = start; ; checkPoint += move) {
      // according to the status to enter the child node
      node = node->childNode[status[checkPoint]];

      // if reach leaf, return type
      if (node->leaf) {
        if (type[0] != NULL) delete type[0];
        if (type[1] != NULL) delete type[1];
        type[0] = new ChessType(node->type[0]);
        type[1] = new ChessType(node->type[1]);

        return;
      }

      // if reach different color, change direction
      if (node->jump)
        break;
    }
}

template <int StatusLength>
template <class DerivedTypeTree>
ChessType* VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree<DerivedTypeTree>::cutSameResultChild(Node *node) {
  ChessType *currentType = NULL;

  if (node->leaf) {
    currentType = node->type;
    return currentType;
  }

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
      else if (currentType[0] != returnType[0] || currentType[1] != returnType[1])
        // if different child has different result, cannot cut this node
        canCut = false;
    }

  if (!canCut) return NULL;
  // cut this node, free all children and set this node's type
  node->type[0] = ChessType(currentType[0]);
  node->type[1] = ChessType(currentType[1]);

  node->leaf = true;

  for (int i = 0; i < 4; ++i)
    if (node->childNode[i] != NULL) {
      delete node->childNode[i];
      node->childNode[i] = NULL;
    }

  return currentType;
}

#endif