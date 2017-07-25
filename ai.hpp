#ifndef AI_H_
#define AI_H_

#include "virtualboard.hpp"
#include "gametree.hpp"
#include <string>
#include <thread>

class AI {
 public:

  /* cycle: nubmer of MCTS cycles */
  AI();

  ~AI();

  // Think of a play, and return the index (row * 15 + col), -1 for pass.
  int think();

  // Plays a chess. Return value indicates winner after this move,
  //  -1 => no one,  0 => black, 1 => white.
  int play(int index);

  /* resets AI for a new game */
  void reset(int level, int rule);

  /* undo "times" move */
  void undo(int times) {
    stopBGThread();
    for (int i = 0; i < times; ++i) tree->undo();
  }

  /* opponent pass */
  void pass() {tree->pass(); }

  /* opponent resign*/
  void resign() { stopBGThread(); }

  /* returns true for black ,false for white. */
  bool whoTurn() {return tree->getCurrentBoard()->whoTurn(); }

  std::string getTreeJSON() { return tree->getTreeJSON(); }

  // Thinks in background. Stops if `controller` changed to false by other threads.
  void thinkInBackground(bool* controller)

 private:
  /* this thread lets EM thinks in the background when it's the users turn */
  std::thread *backgroundThread;
  GameTree *tree;
  VirtualBoard *vb;

  // The max mcts cycle to run in the background.
  static const int MAX_BACKGROUND_CYCLE_

  /* the ai's level */
  int level_;
};

#endif