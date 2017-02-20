#define CHESSBOARD_DIMEN 15

class ChessBoard
{
  public:
    enum STATUS{EMPTY, BLACK, WHITE};

    ChessBoard();

    /* redraw the current chesssboard */
    void invalidate();

    /* puts a new chess */
    void play(STATUS color, int row, int col);

    /* clears the whole game */
    void wipe(bool invalidate);

  private:
    STATUS pointStatus[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
};
