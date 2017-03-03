#include "chesstype.hpp"
#include "status.hpp"

class TypeTree
{
public:
  static void classify(STATUS *status, bool checkForbidden, ChessType *(type[2]));

  static void initialize();

private:
  struct Node
  {
    /* Next point occupied by:
     * 0: black, 1: white, 2:empty 3: bound */
    Node *childNode[4];

    struct ChessType *type[2];

    bool jump;
  };

  static Node *commonTree_root, *forbiddenTree_root;

  /* Depth First Search
   * parameters of the initial call should be:
   * currentLocation: length/2, move = -1 */
  static void dfs(Node *root, STATUS *status, int location, int move, 
    int blackConnect, int whiteConnect, bool blackblock, bool whiteBlock, bool checkForbidden);

  /* cut the tree node that all child has same result */
  static ChessType** cutSameResultChild(Node *root);

  /* copied from chesstypemaker.cpp */
  static ChessType* typeAnalyze(STATUS *status, bool checkForbidden, STATUS color);

  
  /* debugging purposes */

  static int count;

  /* copied from chesstypemaker.cpp, print the status and type */
  static void print(int length, STATUS *status, ChessType **type);

  /* search all the tree and print the leaves */
  static void searchAll(Node* root, STATUS *status, int length, int location, int move);
};