#include "chesstype.hpp"

class TypeTree
{
public:
  //TypeTree();
  //~TypeTree();

  enum STATUS 
  {
    EMPTY = (int)' ', SAME = (int)'O', DIFFERENT = (int)'X', 
    NO_MATTER = (int)'-', ANALYZE_POINT = (int)'*'
  };

  static ChessType* classify(STATUS *status, bool checkForbidden);

  static void initialize();

private:
  struct Node
  {
    /* Next point occupied by:
     * 0:same color, 1:different color, 2:empty */
    Node *childNode[3];

    struct ChessType *type;
  };

  static Node *commonTree_root, *forbiddenTree_root;

  /* Depth First Search
   * parameters of the initial call should be:
   * currentLocation: length/2, move = -1 */
  static void dfs(Node *root, STATUS *status, int currentLocation, 
    int move, int connect, bool checkForbidden);

  /* copied from chesstypemaker.cpp, just for debugging purposes */
  static void print(int length, STATUS *status, ChessType *style);

  /* copied from chesstypemaker.cpp */
  static ChessType* typeAnalyze(STATUS *status, bool checkForbidden);
};