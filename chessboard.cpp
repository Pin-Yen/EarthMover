#include "chessboard.hpp"
#include <iomanip>
#include <assert.h>
#include <string>
#include <iostream>

using namespace std;

ChessBoard::ChessBoard()
{
  blackTurn = true;

  wipe(false);
}

/* prints the current chesssboard */
void ChessBoard::invalidate()
{
  for (int r = 0; r < CHESSBOARD_DIMEN + 2; r++)
    for (int c = 0; c < CHESSBOARD_DIMEN + 2; c++)
    {
      if (r > 0 && r < CHESSBOARD_DIMEN + 1 && c > 0 && c < CHESSBOARD_DIMEN + 1)
      {
        switch (pointStatus[r - 1][c - 1])
        {
          case BLACK:
            printBoard(r, c, BLACK); break;
          case WHITE:
            printBoard(r, c, WHITE); break;
          default:
            printBoard(r, c, EMPTY);
        }
      }
      else
        printBoard(r, c, EMPTY);
    }
}

/* print a part of the board*/
void ChessBoard::printBoard(int row, int col, STATUS chess)
{
  if (row == 0 || row == CHESSBOARD_DIMEN + 1)
    if (col == 0 || col == CHESSBOARD_DIMEN + 1)
      cout << setw(4) << " ";
    else
      cout << setw(4) << (char)(64 + col);
  else if (col == 0 || col == CHESSBOARD_DIMEN + 1)
    cout << setw(4) << row;
  else if (row == 1)
  {
    string c = "";
    c += (col == 1) ? "   " : "───";
    if (chess == EMPTY)
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
      c += (chess == BLACK) ? CHESS_BLACK : CHESS_WHITE;

    cout << c;
  }
  else if (row == CHESSBOARD_DIMEN)
  {
    string c = "";
    c += (col == 1) ? "   " : "───";
    if (chess == EMPTY)
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
      c += (chess == BLACK) ? CHESS_BLACK : CHESS_WHITE;

    cout << c;
  }
  else
  {
    string c = "";
    c += (col == 1) ? "   " : "───";
    if (chess == EMPTY)
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
      c += (chess == BLACK) ? CHESS_BLACK : CHESS_WHITE;

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
  
  records[playNo] = Point(row,col,color);
  playNo++;

  pointStatus[row][col] = color;
  
  blackTurn = !blackTurn;

  invalidate();
}

/* clears the whole game */
void ChessBoard::wipe(bool isInvalidate)
{
  for (int i = 0; i < CHESSBOARD_DIMEN; i++)
    for (int k = 0; k < CHESSBOARD_DIMEN; k++)
      pointStatus[i][k] = EMPTY;

  playNo = 0;

  if (isInvalidate)
    invalidate();
}

bool ChessBoard::isBlackTurn()
{
  return blackTurn;
}

/* search the whole board for winning conditions */
STATUS ChessBoard::judge(){

  const int dir[4][2] = {{0,1},{1,0},{-1,1},{1,1}};
  const int lowerBound[4][2] ={{0,0},{0,0},{4,0},{0,0}}; /* INclude lowerbound when searching */
  const int upperBound[4][2] ={
  {CHESSBOARD_DIMEN,CHESSBOARD_DIMEN-4},
  {CHESSBOARD_DIMEN-4,CHESSBOARD_DIMEN},
  {CHESSBOARD_DIMEN,CHESSBOARD_DIMEN-4},
  {CHESSBOARD_DIMEN-4,CHESSBOARD_DIMEN-4}}; /* EXclude upperbound when searching */

  STATUS targetColor[2] ={BLACK,WHITE};

  for(int color =0; color<2;color++){
    /* for each direction d, start searching from lowerbound, and stop at upperBound */
    for (int d=0;d<4;d++){
      for(int r=lowerBound[d][0];r<upperBound[d][0];r++){
        for(int c=lowerBound[d][1]; c<upperBound[d][1];c++){
          for(int offset=0; offset<5;offset++){
            int checkRow = r+ offset*dir[d][0];
            int checkCol = c+ offset*dir[d][1];

            if(pointStatus[checkRow][checkCol] != targetColor[color])
              break;
            if(offset == 4)
              return targetColor[color];
          }
        }
      }
    }
  }
  
  return EMPTY;
 
}