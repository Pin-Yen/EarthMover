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
  static void classify(const STATUS *status, ChessType *(type[2])) final;

  static void init();

 protected:
  struct Node {
    /* Next point occupied by:
     * 0: black, 1: white, 2:empty 3: bound */
    Node *childNode[4];

    struct ChessType *type[2];

    bool jump;

    Node() {
      jump = false;

      #ifdef DEBUG
      ObjectCounter::registerTypeTreeNode();
      #endif
    }

    ~Node() {
      delete type[0]; delete type[1];

      #ifdef DEBUG
      ObjectCounter::unregisterTypeTreeNode();
      #endif
    }
  };

 private:

  /* cut the tree node that all child has same result */
  static ChessType** cutSameResultChild() final;

  /* Analyze chesstype when reaching leaf in typetree */
  static ChessType* typeAnalyze(STATUS *status, STATUS color, bool checkLevel) final;
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

  DerivedTypeTree::cutSameResultChild();
}

template <int StatusLength>
template <class DerivedTypeTree>
void VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree<DerivedTypeTree>::classify(const STATUS *status, ChessType *(type[2])) {
  Node* node = DerivedTypeTree::root;

  for (int move = -1, start = DerivedTypeTree::classify_length / 2 - 1; ; move = 1, ++start)
    for (int checkPoint = start; ; checkPoint += move) {
      /* according to the status to enter the child node */
      node = node->childNode[status[checkPoint]];

      /* if reach leaf, return type */
      if (node->type[0] != NULL) {
        if (type[0] != NULL) delete type[0];
        if (type[1] != NULL) delete type[1];
        type[0] = new ChessType(node->type[0]);
        type[1] = new ChessType(node->type[1]);

        return;
      }

      /* if reach different color, change direction */
      if (node->jump)
        break;
    }
}

template <int StatusLength>
template <class DerivedTypeTree>
ChessType** VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree<DerivedTypeTree>::cutSameResultChild() {
  ChessType **currentType = NULL;

  if (DerivedTypeTree::root->type[0] != NULL) {
    currentType = DerivedTypeTree::root->type;
    return currentType;
  }

  bool canCut = true;

  for (int i = 0; i < 4; ++i)
    if (DerivedTypeTree::root->childNode[i] != NULL) {
      /* if this child node is not NULL, recursion and get return */
      ChessType** returnType = cutSameResultChild(DerivedTypeTree::root->childNode[i]);

      if (returnType == NULL)
        /* if children cannot be cut, then this node also cannot be cut */
        canCut = false;
      else if (currentType == NULL)
        currentType = returnType;
      else if (*currentType[0] != *returnType[0] || *currentType[1] != *returnType[1])
        /* if different child has different result, cannot cut this node*/
        canCut = false;
    }

  if (!canCut) return NULL;
  /* cut this node, free all children and set this node's type */
  DerivedTypeTree::root->type[0] = new ChessType(currentType[0]);
  DerivedTypeTree::root->type[1] = new ChessType(currentType[1]);

  for (int i = 0; i < 4; ++i)
    if (DerivedTypeTree::root->childNode[i] != NULL)
      delete DerivedTypeTree::root->childNode[i];

  return currentType;
}

template <int StatusLength>
template <class DerivedTypeTree>
ChessType* VirtualBoardGomoku<StatusLength>::Evaluator::TypeTree<DerivedTypeTree>::typeAnalyze(STATUS *status, STATUS color, bool checkLevel) {
  int connect = 1;
  /* check the length of the connection around the analize point
   * under the following, we call this chess group "center group" (CG)
   * for example: --X O*OOX-- ; OOOO* O X
   *                   ^^^^      ^^^^^     */
  for (int move = -1, start = DerivedTypeTree::analyze_length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, checkPoint = start; i < 4; ++i, checkPoint += move) {
      if (status[checkPoint] != color) break;

      ++connect;
    }

  if (connect >= 5) {
    /* CG's length == 5, return 5 */
    return new ChessType(5, 0, 0);
  } else {
    /* CG's length < 5 */

    /* play at the analize point */
    status[DerivedTypeTree::analyze_length / 2] = color;

    /* try to find the left and right bound of CG
     * if it's empty, see this point as new analize point
     * make a new status array and use recursion analize the status */
    ChessType *lType = NULL, *rType = NULL;
    int level = 0;

    for (int move = -1, start = DerivedTypeTree::analyze_length / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, checkPoint = start; count < 4; ++count, checkPoint += move)
        /* if reach CG's bound */
        if (status[checkPoint] != color) {
          ChessType* type = NULL;
          bool blocked = false;

          /* if the bound is an empty point */
          if (status[checkPoint] == EMPTY) {
            /* make a new status array */

            STATUS newStatus[DerivedTypeTree::analyze_length];

            /* transform from origin status */
            for (int i = 0; i < DerivedTypeTree::analyze_length; ++i) {
              int transformation_index = i - (DerivedTypeTree::analyze_length / 2 - checkPoint);

              if (transformation_index < 0 || transformation_index >= DerivedTypeTree::analyze_length)
                /* if out of bound, set it to bound */
                newStatus[i] = BOUND;
              else
                newStatus[i] = status[transformation_index];
            }

            /* recursion analize */
            type = typeAnalyze(newStatus, color, false);
          } else {
            /* if the board of CG is not empty, it means blocked */
            blocked = true;
            type = new ChessType(0, 0, 0);
          }

          if (move == -1) {
            /* left type result */
            if (lType == NULL) {
              lType = type;
              if (!checkLevel || blocked) break;
              if (!lType->life() || lType->length() > 3) break;
            } else {
              if (*lType == *type) {
                ++level;
              } else {
                delete type;
                break;
              }
              delete type;
            }
          } else {
            /* right type result */
            if (rType == NULL) {
              rType = type;
              if (!checkLevel || blocked) break;
              if (!rType->life() || rType->length() > 3) break;
              if (*lType == *rType) {
                ++level;
              } else if (*lType < *rType) {
                level = 0;
              } else {
                break;
              }
            } else {
              if ((*rType == *type) && (*rType >= *lType)) {
                ++level;
              } else {
                delete type;
                break;
              }
              delete type;
            }
          }
        }

    /* restore the analize point to empty */
    status[DerivedTypeTree::analyze_length / 2] = EMPTY;

    /* keep lType > rType */
    if (*lType < *rType)
      std::swap(lType, rType);

    ChessType *returnType;

    if (lType->length() == 5 && rType->length() == 5) {
      /* if left and right will both produce 5 after play at analize point
       * it is a life four type */
      returnType = new ChessType(4, 1, 0);
    } else if (lType->length() == 5) {
      /* if there is only one side produces 5 after play at analize point,
       * it is a dead four type */
      returnType = new ChessType(4, 0, 0);
    } else if (lType->length() == 0) {
      /* if the longer size produces 0 or forbidden point after play at analize point,
       * it is a useless point */
      returnType = new ChessType(0, 0, 0);
    } else {
      /* if left length < 4, return left length - 1
       * (current recursion's result = lower recursion's result - 1) */
      if (checkLevel) {
        if (!lType->life() || lType->length() > 3) {
          returnType = new ChessType(lType->length() - 1, lType->life(), 0);
        } else {
          returnType = new ChessType(lType->length() - 1, lType->life(),
                                     level - (3 - (lType->length() - 1)));
        }
      } else {
        returnType = new ChessType(lType->length() - 1, lType->life(), 0);
      }
    }

    delete lType;
    delete rType;

    return returnType;
  }
}
#endif