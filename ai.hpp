#ifndef AI_H_
#define AI_H_


class AI
{
 public:

  /* set MCTS cycle*/
  AI(int cycle);

  /* initializes board & tree */
  void initialize();

  ~AI();

  /* think of a play, and return it */
  void think(int clientRow, int clientCol, int *Row, int *Col);

 private:
  /* this thread lets EM thinks in the background when it's the users turn */
  std::thread *backgroundThread;
  bool stopBackgroundThread;
  DisplayBoard *board;
  Gametree *gametree;

  /* the number of MCTS cycle*/
  int cycle;
};

#endif