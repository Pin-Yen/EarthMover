#include "chessboard.hpp"
#include <stdio.h>
#include <assert.h>

#define CHESSBOARD_DIMEN 15
#define CHESS_BLACK "○"
#define CHESS_WHITE "●"

// enum STATUS{EMPTY, BLACK, WHITE};

ChessBoard::ChessBoard(){

  /* initialize pointStatus */
  for (int r = 0; r < CHESSBOARD_DIMEN; r++)
    for (int c = 0; c < CHESSBOARD_DIMEN; c++)
      pointStatus[r][c] = EMPTY;

}

/* prints the current chesssboard */
void ChessBoard::invalidate()
{
  for (int r = 0; r < CHESSBOARD_DIMEN; r++)
    for (int c = 0; c < CHESSBOARD_DIMEN; c++)
      switch (pointStatus[r][c])
      {
        case EMPTY:
          if (r == 0)
          {
            if (c == 0)
              printf("┌");
            else if (c == CHESSBOARD_DIMEN)
              printf("┐\n");
            else
              printf("┬");
          }
          else if (r == CHESSBOARD_DIMEN)
          {
            if (c == 0)
              printf("└");
            else if (c == CHESSBOARD_DIMEN)
              printf("┘\n");
            else
              printf("┴");
          }
          else
          {
            if (c == 0)
              printf("├");
            else if (c == CHESSBOARD_DIMEN)
              printf("┤\n");
            else
              printf("┼");
          }
          break;
        case BLACK:
          printf(CHESS_BLACK);
          break;
        case WHITE:
          printf(CHESS_WHITE);
          break;
      }
}

/* puts a new chess */
void ChessBoard::play(STATUS color, int row, int col)
{
  if (pointStatus[row][col] != EMPTY)
    assert(0); 
  else
    pointStatus[row][col] = color;

  invalidate();
}

/* clears the whole game */
void ChessBoard::wipe()
{
  for (int i = 0; i < CHESSBOARD_DIMEN; i++)
    for (int k = 0; k < CHESSBOARD_DIMEN; k++)
      pointStatus[i][k] = EMPTY;

  invalidate();
}
