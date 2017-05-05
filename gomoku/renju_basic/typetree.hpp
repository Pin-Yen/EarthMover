#ifndef TYPE_TREE_H
#define TYPE_TREE_H

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

class VirtualBoard::Evaluator::TypeTree {
 public:
  static void classify(const STATUS *status, ChessType *(type[2]));

  static void initialize();

 private:
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

  static Node* root;

  /* Depth First Search
   * parameters of the initial call should be:
   * currentLocation: length/2, move = -1 */
  static void dfs(Node *root, STATUS *status, int location,
                  int move,  int blackConnect, int whiteConnect,
                  bool blackBlock, bool whiteBlock);

  /* cut the tree node that all child has same result */
  static ChessType** cutSameResultChild(Node *root);

  /* copied from chesstypemaker.cpp */
  static ChessType* typeAnalyze(STATUS *status, STATUS color, bool checkLevel);

  static const int analyze_length = 11, classify_length = 10;
};

#endif