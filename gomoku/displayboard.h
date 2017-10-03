#ifndef GOMOKU_DISPLAYBOARD_H_
#define GOMOKU_DISPLAYBOARD_H_

class DisplayBoard {
 public:
  DisplayBoard();

  // get user's input
  void getInput(int *row, int *col) const;

  // Puts a new chess.
  // If the point is not empty or out of bound then return false.
  bool play(int row, int col);

  // clears the whole game
  void wipe();

  // get who turn, 0 = black, 1 = white
  bool whoTurn() const { return (playNo_ & 1); }

  int playNo() const { return playNo_; }

 private:
  static const int DIMEN = 15;
  static const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

  // point array
  int point_[DIMEN][DIMEN];

  // the total number of plays
  int playNo_;

  // print the current chesssboard
  void invalidate() const;

  // print a part of the board
  void printBoard(int row, int col, int color) const;
};

#endif  // GOMOKU_DISPLAYBOARD_H_
