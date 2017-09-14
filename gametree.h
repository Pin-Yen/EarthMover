#ifndef GAMETREE_H_
#define GAMETREE_H_

#include <string>

#include "lib/json.h"
#include "memorypool.h"
#include "virtualboard.h"

class GameTree {
 public:
  GameTree();
  ~GameTree();

  // Initialize GameTree.
  void init(VirtualBoard* board);

  // Monty Carlo Tree Search.
  // Keep searching until reach 'cycle'.
  // Return false if the search ends prematurely.
  bool mcts(int cycle);

  // Monty Carlo Tree Search.
  // Stop if the point that count most times > 'minCount',
  // will check it after each batch.
  void mcts(int batch, int minCount);

  // Monty Carlo Tree Search.
  // Keep searching until 'thinking' becomes false,
  void mcts(const bool* continueThinking);
  // or reach the 'maxCycle'.
  void mcts(int maxCycle, const bool* thinking);

  // Monty Carlo Tree Search.
  // Line 26's multi-thread version.
  void mcts(int threadAmount, int batch, int minCount);
  // Line 32's multi-thread version.
  void mcts(int threadAmount, int maxCycle, const bool* thinking);

  // get the child that has highest playout from current node
  int mctsResult() const;

  // called when a REAL point is played, updates the currentRoot
  // returns 1 if wins after play, -1 if lose
  int play(int index);

  // pass
  void pass();

  // undo
  void undo();

  // Returns the whole tree in JSON format
  std::string getTreeJSON();

  // getter
  VirtualBoard* getCurrentBoard() { return currentBoard_; }

 private:
  class Node;

  // MCTS function.
  // keep select the child node from the current node,
  // until reach the leaf or a winning node.
  // Puts the result to node,
  // returns -2 for a no result leaf node, -1 for FULL chessboard,
  // 0 for losing, 1 for winning.
  int selection(Node** node, VirtualBoard* board);

  // MCTS function
  // simulate the game at most maxDepth move,
  // and return who win (black = 0, white = 1, tie = -1)
  int simulation(VirtualBoard* board) const;

  // MCTS function
  // back propagation form node, until reach the current node
  void backProp(Node* node, bool result);
  void backProp(Node* node);

  // Copy all children in 'srcNode' to 'destNode'.
  void copyAllChildren(const Node* srcNode, Node* destNode);

  // Copy tree, new tree's root is the current node of 'source'.
  void copy(const GameTree* source);

  // Merge all tree into this.
  void mergeTree(GameTree* tree);

  // Merge all children in 'mergedNode' into 'mergingNode'
  void mergeAllChildren(Node* mergingNode, const Node* mergedNode);

  void minusTree(GameTree* beMinusTree, const GameTree* minusTree);

  void minusAllChildren(Node* beMinusNode, const Node* minusNode);

  // returns the part of the tree below 'node' in JSON format
  nlohmann::json getSubTreeJSON(Node *node, bool whoTurn);

  Node *root_, *currentNode_;

  VirtualBoard* currentBoard_;

  MemoryPool pool_;
};

#endif  // GAMETREE_H_
