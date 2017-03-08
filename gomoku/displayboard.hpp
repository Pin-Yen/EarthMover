class DisplayBoard
{
public:
  DisplayBoard();

  /* get user's input */
  void getInput(int &row, int &col);

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
  static const int CHESSBOARD_DIMEN = 15;
  const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

  /* point array */
  int point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  /* the total number of plays */
  int playNo;

  /* print a part of the board */
  void printBoard(int row, int col, int color);
};