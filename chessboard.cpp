#include "chessboard.hpp"
#include <stdio.h>
#include <iomanip>
#include <assert.h>
#include <string>
#include <iostream>

using namespace std;

//#define CHESS_BLACK "○"
//#define CHESS_WHITE "●"


ChessBoard::ChessBoard()
{
  wipe(false);
}

/* prints the current chesssboard */
void ChessBoard::invalidate()
{
  for (int r = 0; r < CHESSBOARD_DIMEN + 2; r++)
    for (int c = 0; c < CHESSBOARD_DIMEN + 2; c++)
    {
      if (c > 0 && c < CHESSBOARD_DIMEN + 1)
      {
        switch (pointStatus[r - 1][c - 1])
        {
          case BLACK:
            printBoard(r, c, 'X'); break;
          case WHITE:
            printBoard(r, c, 'O'); break;
          default:
            printBoard(r, c, -1);
        }
      }
      else
        printBoard(r, c, -1);
    }
}

void ChessBoard::printBoard(int row, int col, char chess)
{
  if (row == 0 || row == CHESSBOARD_DIMEN + 1)
    if (col == 0 || col == CHESSBOARD_DIMEN + 1)
      cout << setw(4) << " ";
    else
      cout << setw(4) << col;
  else if (col == 0 || col == CHESSBOARD_DIMEN + 1)
    cout << setw(4) << row;
  else if (row == 1)
  {
    string c = "";
    c += (col == 1) ? "   " : "───";
    if (chess == -1)
    {
      switch (col)
      {
        case 1:
          c += "┌"; break;
        case CHESSBOARD_DIMEN:
          c += "┐"; break;
        default:
          c += "┬";
      }
    }
    else
      c += chess;

    cout << c;
  }
  else if (row == CHESSBOARD_DIMEN)
  {
    string c = "";
    c += (col == 1) ? "   " : "───";
    if (chess == -1)
    {
      switch (col)
      {
        case 1:
          c += "└"; break;
        case CHESSBOARD_DIMEN:
          c += "┘"; break;
        default:
          c += "┴";
      }
    }
    else
      c += chess;

    cout << c;
  }
  else
  {
    string c = "";
    c += (col == 1) ? "   " : "───";
    if (chess == -1)
    {
      switch (col)
      {
        case 1:
          c += "├"; break;
        case CHESSBOARD_DIMEN:
          c += "┤"; break;
        default:
          c += "┼";
      }
    }
    else
      c += chess;

    cout << c;
  }

  if (col == CHESSBOARD_DIMEN + 1)
  {
    cout << "\n    ";

    if (row > 0 && row < CHESSBOARD_DIMEN)
      for (int i = 0; i < CHESSBOARD_DIMEN; i++)
        cout << "   │";

    cout << "\n";
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
void ChessBoard::wipe(bool isInvalidate)
{
  for (int i = 0; i < CHESSBOARD_DIMEN; i++)
    for (int k = 0; k < CHESSBOARD_DIMEN; k++)
      pointStatus[i][k] = EMPTY;

  if (isInvalidate)
    invalidate();
}
