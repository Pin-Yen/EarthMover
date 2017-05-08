#ifndef AI_H_
#define AI_H_

#include "gomoku/displayboard.hpp"
#include "gametree.hpp"
#include <thread>

class AI
{
 public:

  /* cycle: nubmer of MCTS cycles */
  AI(int level);

  ~AI();

  /* puts a new point on displayboard
   * returns false if the point is invalid */
  bool displayNewPoint(int row, int col);

  /* think of a play, and return it. */
  void think(int *Row, int *Col);

  /* plays a new point, returns true if someone wins after this move.
   * triggerBackgroundThread: true: start bg thread */
  bool play(int row, int col, bool triggerBackgroundThread);

  /* resets AI for a new game */
  void reset();

  /* returns true for black ,false for white. */
  bool whoTurn();

 private:
  /* this thread lets EM thinks in the background when it's the users turn */
  std::thread *backgroundThread;
  bool stopBackgroundThread;
  GameTree *tree;

  /* starts background thread */
  void startBGThread();

  /* stops background thread */
  void stopBGThread();

  /* the ai's level */
  int level_;
};

#endif