#ifndef AI_H_
#define AI_H_

#include "virtualboard.hpp"
#include "gametree.hpp"
#include <thread>

class AI {
 public:

  /* cycle: nubmer of MCTS cycles */
  AI();

  ~AI();

  /* puts a new point on displayboard
   * returns false if the point is invalid */
  bool displayNewPoint(int row, int col);

  /* think of a play, and return it. */
  void think(int *Row, int *Col);

  /* plays a new point
   * return value: 1: self-winning, -1: opp-winning, 0 no winning
   * triggerBackgroundThread: true: start bg thread */
  int play(int row, int col, bool triggerBackgroundThread);

  /* resets AI for a new game */
  void reset(int level, int rule);

  /* opponent pass */
  //void pass() { }

  /* opponent resign*/
  void resign() { stopBGThread(); }

  /* returns true for black ,false for white. */
  bool whoTurn();

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