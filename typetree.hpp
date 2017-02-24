#include "style.hpp"

class TypeTree
{
public:
	TypeTree();
	~TypeTree();

	enum STATUS 
	{
		EMPTY = (int)' ', SAME = (int)'O', DIFFERENT = (int)'X', 
		NO_MATTER = (int)'-', ANALYZE_POINT = (int)'*'
	};

	Style* classify(STATUS *status);

private:

	const int length = 9;
	const bool checkLongConnect = false;

	struct Node{

		/* Next point occupied by:
		 * 0:same color, 1:different color, 2:empty */
		Node *childNode[3];

		struct Style *style;
	};

	Node root;

	/* Depth First Search
	 * parameters of the initial call should be:
	 * currentLocation: length/2, move = -1 */
	void dfs(Node *root, STATUS *status, int currentLocation, int move);

	/* copied from printstyle.cpp, just for debugging purposes */
	void print(int length, STATUS *status, Style *style);

	/* copied from printstyle.cpp */
	Style * styleAnalyze(int length, STATUS *status, bool checkLongConnect);


};