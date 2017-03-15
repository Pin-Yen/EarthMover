#ifndef GAMENODE_HPP
#define GAMENODE_HPP
class GameNode
{
public:

  /* consturctor for root */
  GameNode();
  /* constructor for node (WITHOUT root node) */
  GameNode(GameNode *parentNode, int row, int col);

  ~GameNode();

  /* result: -1 = tie, 0 = black win, 1 = white win */
  void update(int result);

  /* get childNode */
  /* note: if the child node is NULL, it will return NULL */
  Node* getChildNode(int r, int c) { return childNode[r][c]; }

  /* get the Upper Confidence Bound value form child node */
  double getUCBValue(int r, int c)
  {
    //TODO
  }

private:
    Node *childNode[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

    int childProb[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
    /* 0 = black, 1 = white, 2 = total */
    int playout[3];
    int row, col;

    VirtualBoard* board;
    Node *parent;
};
#endif