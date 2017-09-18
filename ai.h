#ifndef AI_H_
#define AI_H_

#include <string>
#include <thread>
#include "gametree.h"
#include "virtualboard.h"

class AI {
 public:
  AI();

  ~AI();

  // Think of a play, and return the index (row * 15 + col), -1 for pass.
  int think();

  // Plays a chess. Return value indicates winner after this move,
  //  -1 => no one,  0 => black, 1 => white.
  int play(int index);

  // resets AI for a new game
  void reset(int level, int rule);

  // undo "times" move
  void undo(int times) {
    for (int i = 0; i < times; ++i) tree->undo();
  }

  // opponent pass
  void pass() {tree->pass(); }

  // opponent resign
  void resign() {}

  // returns false for black, true for white.
  bool whoTurn() {return tree->getCurrentBoard()->whoTurn(); }

  std::string getTreeJSON() { return tree->getTreeJSON(); }

  // Thinks in background.
  // Stops if `controller` changed to false by other threads.
  void thinkInBackground(bool* controller);

 private:
  GameTree *tree;

  // The max mcts cycle to run in the background.
  static const int MAX_BACKGROUND_CYCLE_ = 100000;

  // the ai's level
  int level_;
};

#endif  // AI_H_
