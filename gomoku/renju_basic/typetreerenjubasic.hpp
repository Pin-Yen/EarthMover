#ifndef TYPE_TREE_RENJU_BASIC_H
#define TYPE_TREE_RENJU_BASIC_H

#include "../typetree.hpp"

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

class VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic : public VirtualBoardGomoku<10>::Evaluator::TypeTree<VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic> {
 // Declare base class as friend to use CRTP
 friend class VirtualBoardGomoku<10>::Evaluator::TypeTree< VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic >;

 private:
  static bool isInit;

  static Node* root;

  // Constructs a untrimmed typetree (by calling dfs). Should be trimmed before use.
  static void plantTree();

  // Depth First Search
  // parameters of the initial call should be:
  // currentLocation: length/2, move = -1
  static void dfs(Node *node, STATUS *status, int location, int move, int blackConnect, int whiteConnect,
                  bool blackBlock, bool whiteBlock);

  // analyze chesstype, only call by dfs
  static ChessType typeAnalyze(STATUS *status, STATUS color, bool checkLevel);
};

#endif