#include "virtualboard.hpp"

class GameTree
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  GameTree();

  void play(int row, int col);
private:
  struct Node
  {
    Node *childNode[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

    VirtualBoard* board;
  };

  Node* currentNode;
};