#include "gamenode.hpp"

GameNode::GameNode()
{
	for (int r = 0; r<CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c<CHESSBOARD_DIMEN; ++c)
    {
      childNode[r][c] = NULL;
      childProp[r][c] = 0;
    }

  /* initialize playout counters to 0 */
  for(int i = 0 ; i < 3; ++i)
    playout[i] = 0;

	/* initialize board */
	board = new VirtualBoard();
	
	/* initiaize parent node */
  parent = parentNode;
}

GameNode::GameNode(GameNode *parentNode, int row, int col)
{
  /* initialize all childNodes to NULL, all childProp to 0 */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
    {
      childNode[r][c] = NULL;
      childProp[r][c] = 0;
    }

  /* initialize playout counters to 0 */
  for(int i = 0 ; i < 3; ++i)
    playout[i] = 0;

  /* initialize board */
  board = new VirtualBoard(parentNode->board);
  board->play(row, col);

  /* initiaize parent node */
  parent = parentNode;
}

GameNode::~GameNode()
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      if (childNode[r][c] != NULL)
        delete childNode[r][c];
}

GameNode::update(int result)
{
	++playout[2];
	if (result != -1) ++playout[result];
}