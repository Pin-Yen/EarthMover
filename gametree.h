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

  // clear the whole gametree
  void reset(VirtualBoard* board);

  // Monty Carlo Tree Search
  // keep searching until reach the max cycle
  void MCTS(int maxCycle);

  // Monty Carlo Tree Search
  // keep searching until total select times reach "minCycle"
  // and the point that select most times reach "minMostTimesCycle"
  void MCTS(int minCycle, int minMostTimesCycle);

  /* Monty Carlo Tree Search
   * keep searching until reach the max cycle, or continueThinking becomes false */
  void MCTS(int maxCycle, const bool* continueThinking);

  // get the child that has highest playout from current node
  int MCTSResult() const;

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
  int selection(Node** node, VirtualBoard* board) const;

  // MCTS function
  // simulate the game at most maxDepth move,
  // and return who win (black = 0, white = 1, tie = -1)
  int simulation(VirtualBoard* board) const;

  // MCTS function
  // back propagation form node, until reach the current node
  void backProp(Node* node, bool result);
  void backProp(Node* node);

  // returns the part of the tree below 'node' in JSON format
  nlohmann::json getSubTreeJSON(Node *node, bool whoTurn);

  Node *root_, *currentNode_;

  VirtualBoard* currentBoard_;

  static MemoryPool pool_;
};

#endif  // GAMETREE_H_
