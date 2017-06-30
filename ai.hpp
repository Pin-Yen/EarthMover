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

  /* think of a play, and return it. */
  int think();

  /* plays a new point
   * return value: 1: self-winning, -1: opp-winning, 0 no winning
   * triggerBackgroundThread: true: start bg thread */
  int play(int index, bool triggerBackgroundThread);

  /* resets AI for a new game */
  void reset(int level, int rule);

  /* undo "times" move */
  void undo(int times) {
    stopBGThread();
    for (int i = 0; i < times; ++i) tree->undo();
  }

  /* opponent pass */
  void pass() { stopBGThread(); tree->pass(); }

  /* opponent resign*/
  void resign() { stopBGThread(); }

  /* returns true for black ,false for white. */
  bool whoTurn();

  std::string getTreeJSON() { return tree->getTreeJSON(); }

 private:
  /* this thread lets EM thinks in the background when it's the users turn */
  std::thread *backgroundThread;
  bool stopBackgroundThread;
  GameTree *tree;
  VirtualBoard *vb;

  /* starts background thread */
  void startBGThread();

  /* stops background thread */
  void stopBGThread();

  /* the ai's level */
  int level_;
};

#endif