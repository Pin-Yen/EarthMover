class GameTree::Node
{
public:
  /* consturctor for root */
  Node();
  /* constructor for node (WITHOUT root node) */
  Node(Node *parentNode, int row, int col);

  ~Node();

  /* result: -1 = tie, 0 = black win, 1 = white win */
  void update(int result);

  /* get the Upper Confidence Bound value form child node */
  double getUCBValue(int r, int c);

  Node* getParent() { return parent; }

  Node *childNode[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
private:
  /* 0 = black, 1 = white, 2 = total */
  int playout[3];
  int row, col;

  VirtualBoard* board;
  Node *parent;
};