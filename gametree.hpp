#ifndef GAME_TREE_H
#define GAME_TREE_H

#include "virtualboard.hpp"

class GameTree {
 public:
  static const int CHESSBOARD_DIMEN = 15;

  GameTree();
  ~GameTree();

  /* clear the whole gametree */
  void reset(VirtualBoard* board);

  /* Monty Carlo Tree Search
   * keep searching until reach the max cycle */
  void MCTS(int maxCycle);

  /* Monty Carlo Tree Search
   * keep searching until total select times reach "minCycle"
     and the point that select most times reach "minMostTimesCycle" */
  void MCTS(int minCycle, int minMostTimesCycle);

  /* Monty Carlo Tree Search
   * keep searching until reach the max cycle, or stop become true */
  void MCTS(int maxCycle, bool &stop);

  /* get the child that has highest playout from current node */
  void MCTSResult(int &row, int &col) const;

  /* called when a REAL point is played, updates the currentRoot
   * returns 1 if wins after play, -1 if lose */
  int play(int row, int col);

  /* debugger */
  VirtualBoard* getCurrentBoard() { return currentBoard; }
 private:
  /* nested class */
  class Node;

  /* MCTS function
   * keep select the child node from the current node, until reach the leaf or a winning node
   * puts the result to node,
   * returns -2 for a no result leaf node, -1 for FULL chessboard, 0 for losing, 1 for winning */
  int selection(Node** node, VirtualBoard* board) const;

  /* MCTS function
   * simulate the game at most maxDepth move,
   * and return who win (black = 0, white = 1, tie = -1) */
  int simulation(VirtualBoard* board) const;

  /* MCTS function
   * back propagation form node, until reach the current node */
  void backProp(Node* node, bool result);
  void backProp(Node* node);

  Node *root, *currentNode;

  VirtualBoard* currentBoard;
};

#endif