#ifndef GOMOKU_RENJU_BASIC_TYPETREERENJUBASIC_H_
#define GOMOKU_RENJU_BASIC_TYPETREERENJUBASIC_H_

#include "../typetree.h"

class VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic :
    public VirtualBoardGomoku<10>::Evaluator::TypeTree<
    VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic> {
  // Declare base class as friend to use CRTP
  friend class VirtualBoardGomoku<10>::Evaluator::TypeTree<
      VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic>;

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
                  int blackConnect, int whiteConnect,
                  bool blackBlock, bool whiteBlock);

  // analyze chesstype, only call by dfs
  static SingleType typeAnalyze(STATUS *status, STATUS color, bool checkLevel);
};

#endif  // GOMOKU_RENJU_BASIC_TYPETREERENJUBASIC_H_
