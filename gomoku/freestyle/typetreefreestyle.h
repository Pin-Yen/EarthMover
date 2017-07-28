#ifndef GOMOKU_FREESTYLE_TYPETREEFREESTYLE_H_
#define GOMOKU_FREESTYLE_TYPETREEFREESTYLE_H_

#include "../typetree.h"

#ifdef DEBUG
#include "../../objectcounter.h"
#endif

class VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle :
    public VirtualBoardGomoku<8>::Evaluator::TypeTree<
    VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle> {
  // Declare base class as friend to use CRTP
  friend class VirtualBoardGomoku<8>::Evaluator::TypeTree<
      VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle>;

 private:
  static bool isInit;

  static Node* root;

  // Constructs a untrimmed typetree (by calling dfs).
  // Should be trimmed before use.
  static void plantTree();

  // Depth First Search, given a root and status seed. Grows the tree.
  // parameters of the initial call should be:
  // currentLocation: length/2, move = -1
  static void dfs(Node *node, STATUS *status, int location, int move,
                  bool blackBlock, bool whiteBlock);

  // analyze chesstype, only call by dfs
  static SingleType typeAnalyze(STATUS *status, STATUS color, bool checkLevel);
};

#endif
