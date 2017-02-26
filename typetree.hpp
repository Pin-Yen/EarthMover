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
  static void dfs(Node *root, STATUS *status, int location, 
    int move, int connect, bool checkForbidden);

  /* cut the tree node that all child has same result*/
  static ChessType* cutSameResultChild(Node *root);

  /* copied from chesstypemaker.cpp */
  static ChessType* typeAnalyze(STATUS *status, bool checkForbidden);

  
  /* debugging purposes*/

  static int count;

  /* copied from chesstypemaker.cpp, print the status and type */
  static void print(int length, STATUS *status, ChessType *type);

  /* search all the tree and print the leaves*/
  static void searchAll(Node* root, STATUS *status, int length, 
    int location, int move);
};