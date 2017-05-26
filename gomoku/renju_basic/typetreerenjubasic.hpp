#ifndef TYPE_TREE_RENJU_BASIC_H
#define TYPE_TREE_RENJU_BASIC_H
#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif
#include "../typetree.hpp"
class VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic : public VirtualBoardGomoku<10>::Evaluator::TypeTree<VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic> {
 // Declare base class as friend to use CRTP
 friend class VirtualBoardGomoku<10>::Evaluator::TypeTree< VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic >;

 private:

  static Node* root;

  static bool isInit;
  /* Constructs a untrimmed typetree (by calling dfs). Should be trimmed before use. */
  static void plantTree();

  /* Depth First Search
   * parameters of the initial call should be:
   * currentLocation: length/2, move = -1 */
  static void dfs(Node *root, STATUS *status, int location,
                  int move,  int blackConnect, int whiteConnect,
                  bool blackBlock, bool whiteBlock);




  static const int analyze_length = 11, classify_length = 10;
};

#endif