#ifndef AI_H_
#define AI_H_

#include "gomoku/displayboard.hpp"
#include "gametree.hpp"
#include <thread>

class AI
{
 public:

  /* cycle: nubmer of MCTS cycles */
  AI(int cycle, DisplayBoard *board);

  ~AI();

  /* puts a new point on displayboard
   * returns false if the point is invalid */
  bool displayNewPoint(int row, int col);

  /* think of a play, and return it. */
  void think(int *Row, int *Col);

  /* plays a new point, returns true if someone wins after this move.
   * triggerBackgroundThread: true: start bg thread */
  bool play(int row, int col, bool triggerBackgroundThread);

  bool whoTurn() {return (bool)board->whoTurn();}

  /* stops background thread */
  void stopBGThread();

  /* resets AI for a new game */
  void reset();

  /* change number of MCTS cycle. */
  void getCycle(int cylce);

 private:
  /* this thread lets EM thinks in the background when it's the users turn */
  std::thread *backgroundThread;
  bool stopBackgroundThread;
  DisplayBoard *board;
  GameTree *tree;

  /* the number of MCTS cycle*/
  int cycle;
};

#endif