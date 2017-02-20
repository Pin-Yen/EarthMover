#define CHESSBOARD_DIMEN 15
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

    /* puts a new chess */
    void play(STATUS color, int row, int col);

    /* clears the whole game */
    void wipe(bool isInvalidate);

    /* get is black turn or not*/
    bool isBlackTurn();

  private:
    const char CHESS_BLACK = 'X', CHESS_WHITE = 'O';

    STATUS pointStatus[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];

    std::array<Point,CHESSBOARD_DIMEN*CHESSBOARD_DIMEN> records;

    bool blackTurn;

    /* the total number of plays */
    int playNo;

    /* print a part of the board*/
    void printBoard(int row, int col, STATUS chess);
};
 
