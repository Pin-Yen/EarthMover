#define CHESSBOARD_DIMEN 15
//#define CHESS_BLACK "X"
//#define CHESS_WHITE "O"
#include <array>
#include "point.hpp"
class ChessBoard
{
  public:

    #ifndef STATUS_ENUM
    #define STATUS_ENUM
    enum STATUS{EMPTY, BLACK, WHITE};
    #endif

    ChessBoard();

    /* print the current chesssboard */
    void invalidate();

    /* print a part of the board*/
    void printBoard(int row, int col, STATUS chess);

    /* puts a new chess */
    void play(STATUS color, int row, int col);

    /* clears the whole game */
    void wipe(bool isInvalidate);

  private:
    STATUS pointStatus[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

    const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

    std::array<Point,CHESSBOARD_DIMEN*CHESSBOARD_DIMEN> records;

    /* the total number of plays */
    int playNo;
};
 