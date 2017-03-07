class DisplayBoard
{
public:
  static const int CHESSBOARD_DIMEN = 15;

  DisplayBoard();

  /* print the current chesssboard */
  void invalidate();

  /* puts a new chess, if the ponit is not empty then return false */
  bool play(int row, int col);

  /* clears the whole game */
  void wipe();

  /* get is black turn or not */
  bool isBlackTurn();

  /* search the area surrounding (row, col) for winning conditions */
  bool judge(int row, int col);

private:
  const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

  /* point array */
  int point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  /* the total number of plays */
  int playNo;

  /* print a part of the board */
  void printBoard(int row, int col, int color);
};