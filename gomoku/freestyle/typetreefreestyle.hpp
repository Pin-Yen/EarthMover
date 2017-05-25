#ifndef TYPE_TREE_FREESTYLE_H
#define TYPE_TREE_FREESTYLE_H

#include "../typetree.hpp"
#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

class VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle : public VirtualBoardGomoku<8>::Evaluator::TypeTree<VirtualBoardFreeStyle::EvaluatorFreeStyle::TypeTreeFreeStyle>{
 public:

 private:
  static bool isInit;

  static Node* root;

  /* Constructs a ??? typetree (by calling dfs). Should be trimmed before use. */
  static void plantTree();

  /* Depth First Search, given a root and status seed. Grows the tree.
   * parameters of the initial call should be:
   * currentLocation: length/2, move = -1 */
  static void dfs(Node *root, STATUS *status, int location, int move,
                  bool blackBlock, bool whiteBlock);

  static const int analyze_length = 9, classify_length = 8;
};

#endif