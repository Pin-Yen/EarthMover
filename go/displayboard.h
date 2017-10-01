#ifndef GO_DISPLAYBOARD_H_
#define GO_DISPLAYBOARD_H_

#include <array>
#include <vector>

class DisplayBoard {
 public:
  DisplayBoard();

  // get user's input
  void getInput(int *row, int *col) const;

  // Puts a new chess.
  // If the point is not empty or out of bound then return false.
  bool play(int row, int col);

  // clears the whole game
  void clear();

  // get who turn, 0 = black, 1 = white
  bool whoTurn() const { return (playNo_ & 1); }

  int playNo() const { return playNo_; }

  void getBoard(int dest[19][19]) const {
    for (int r = 0; r < 19; ++r) {
      for (int c = 0; c < 19; ++c) {
        dest[r][c] = board[r][c];
      }
    }
  }

 private:
  static const int CHESSBOARD_DIMEN = 19;
  static const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

  bool outOfBound(int row, int col) {
    return (row >= CHESSBOARD_DIMEN || row < 0 ||
            col >= CHESSBOARD_DIMEN || col < 0);
  }

  // print the current chesssboard
  void invalidate() const;

  // print a part of the board
  void printBoard(int row, int col, int color) const;

  bool isStarPoint(int row, int col) const {
    return ((row) == 4 || (row) == 10 || (row) == 16) &&
           ((col) == 4 || (col) == 10 || (col) == 16);
  }

  void removeIfDead(int row, int col, int color);

  bool noLiberty(int row, int col, int color,
                 std::vector<std::array<int, 2>> *group);

  // point array
  int board[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

  // the total number of plays
  int playNo_;
};

#endif  // GO_DISPLAYBOARD_H_
