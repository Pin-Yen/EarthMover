#ifndef GOMOKU_DISPLAYBOARD_H_
#define GOMOKU_DISPLAYBOARD_H_

class DisplayBoard {
 public:
  DisplayBoard();

  /* get user's input */
  void getInput(int *row, int *col) const;

  /* puts a new chess, if the point is not empty or outofbound then return false */
  bool play(int row, int col);

  /* clears the whole game */
  void wipe();

  /* get who turn, 0 = black, 1 = white */
  bool whoTurn() const { return (playNo_ & 1); }

  int playNo() const { return playNo_; }

 private:
  static const int CHESSBOARD_DIMEN = 15;
  static const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

  /* point array */
  int point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  /* the total number of plays */
  int playNo_;

  /* print the current chesssboard */
  void invalidate() const;

  /* print a part of the board */
  void printBoard(int row, int col, int color) const;
};

#endif
