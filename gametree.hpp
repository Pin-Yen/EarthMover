#include "virtualboard.hpp"
#include "gamenode.hpp"

class GameTree
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  GameTree();

  void play(int row, int col);

  
private:

  Node *root *currentNode;
};