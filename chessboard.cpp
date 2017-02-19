#include "chessboard.hpp"
#include <stdio.h>
#include <boo>
#include <assert.h>

#define CHESSBOARD_DIMEN 15
#define CHESS_BLACK "○"
#define CHESS_WHITE "●"

enum STATUS{EMPTY, BLACK, WHITE};

void ChessBoard::ChessBoard()
{
    wipe(false);
}

/* prints the current chesssboard */
void ChessBoard::invalidate()
{
    for (int r = 0; r < CHESSBOARD_DIMEN; r++)
        for (int c = 0; c < CHESSBOARD_DIMEN; c++)
        {
            switch (pointStatus[r][c])
            {
                case BLACK:
                    printf(CHESS_BLACK); break;
                case WHITE:
                    printf(CHESS_WHITE); break;
                default:
                    switch (r)
                    {
                        case 0:
                            switch (c)
                            {
                                case 0:
                                    printf("┌"); break;
                                case CHESSBOARD_DIMEN - 1:
                                    printf("┐"); break;
                                default:
                                    printf("┬");
                            }
                            break;
                        case CHESSBOARD_DIMEN - 1:
                            switch (c)
                            {
                                case 0:
                                    printf("└"); break;
                                case CHESSBOARD_DIMEN - 1:
                                    printf("┘"); break;
                                default:
                                    printf("┴");
                            }
                            break;
                        default:
                            switch (c)
                            {
                                case 0:
                                    printf("├"); break;
                                case CHESSBOARD_DIMEN - 1:
                                    printf("┤"); break;
                                default:
                                    printf("┼");
                            }
                            break;
                    }
            }

            if (c == CHESSBOARD_DIMEN - 1)
                printf("\n");
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
void ChessBoard::wipe(bool invalidate)
{
    for (int i = 0; i < CHESSBOARD_DIMEN; i++)
        for (int k = 0; k < CHESSBOARD_DIMEN; k++)
            pointStatus[i][k] = EMPTY;

        if (invalidate)
            invalidate();
}
