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

  /* initializes board & tree */
  void initialize();

  ~AI();

  /* puts a new point on displayboard
   * returns false if the point is invalid */
  bool displayNewPoint(int row, int col);

  /* think of a play, and return it.
   * (clientRow, clientCol) is the previous play, and is (-1,-1) if doesn't exist */
  void think(int clientRow, int clientCol, int *Row, int *Col);

  /* plays a new point, returns true if someone wins after this move */
  bool play(int row, int col);

  bool getWhoTurn() {return (bool)board->whoTurn();}

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