class TypeTree {
 public:
  static void classify(const STATUS *status, ChessType *(type[2]));

  static void initialize();

  /* debugging purposes */
  static int counter;

 private:
  struct Node {
    /* Next point occupied by:
     * 0: black, 1: white, 2:empty 3: bound */
    Node *childNode[4];

    struct ChessType *type[2];

    bool jump;

    Node() {
      jump = false;

      ObjectCounter::registerTypeTreeNode();
    }

    ~Node() {
      delete type[0]; delete type[1];

      ObjectCounter::unregisterTypeTreeNode();
    }
  };

  static Node* root;

  /* Depth First Search
   * parameters of the initial call should be:
   * currentLocation: length/2, move = -1 */
  static void dfs(Node *root, STATUS *status, int location, int move,
                  bool blackBlock, bool whiteBlock);

  /* cut the tree node that all child has same result */
  static ChessType** cutSameResultChild(Node *root);

  /* copied from chesstypemaker.cpp */
  static ChessType* typeAnalyze(STATUS *status, STATUS color, bool checkLevel);

  static const int analyze_length = 9, classify_length = 8;

  /* debugging purposes */

  /* copied from chesstypemaker.cpp, print the status and type */
  static void print(STATUS *status, ChessType **type);

  /* search all the tree and print the leaves */
  static void searchAll(Node* root, STATUS *status, int location, int move);
};