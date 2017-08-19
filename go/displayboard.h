#ifndef GO_DISPLAYBOARD_H_
#define GO_DISPLAYBOARD_H_

class DisplayBoard {
 public:
  DisplayBoard();

  // get user's input
  void getInput(int *row, int *col) const;

  // Puts a new chess.
  // If the point is not empty or out of bound then return false.
  bool play(int row, int col);

  // get who turn, 0 = black, 1 = white
  bool whoTurn() const { return (playNo_ & 1); }

  int playNo() const { return playNo_; }

 private:
  static const int CHESSBOARD_DIMEN = 19;
  static const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

  // print the current chesssboard
  void invalidate() const;

  // print a part of the board
  void printBoard(int row, int col, int color) const;

  bool isStarPoint(int row, int col) const {
    return ((row) == 4 || (row) == 10 || (row) == 16) &&
           ((col) == 4 || (col) == 10 || (col) == 16);
  }

  // point array
  int point[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  // the total number of plays
  int playNo_;
};

#endif  // GO_DISPLAYBOARD_H_
