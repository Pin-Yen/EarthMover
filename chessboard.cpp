#include "chessboard.hpp"
#include <iomanip>
#include <string>
#include <iostream>

using namespace std;

ChessBoard::ChessBoard()
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(r, c);

  /* index: 0→ 1↓ 2↗ 3↘*/
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
    {

      for (int d = 0; d < 4; ++d)
        for (int offset = -5; offset < 6; ++offset)
        {
          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          STATUS* status;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          {
            STATUS s = BOUND; status = &s;
          }
          else
            status = &(point[r][c]->color);

          point[r][c]->setDirStatus(d, offset + 5, status);
        }
    }

  wipe(false);

}

/* prints the current chesssboard */
void ChessBoard::invalidate()
{
  for (int r = 0; r < CHESSBOARD_DIMEN + 2; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN + 2; ++c)
    {
      if (r > 0 && r < CHESSBOARD_DIMEN + 1 && c > 0 && c < CHESSBOARD_DIMEN + 1)
        printBoard(r, c, point[r - 1][c - 1]->color);
      else
        printBoard(r, c, EMPTY);
    }
}

/* print a part of the board*/
void ChessBoard::printBoard(int row, int col, STATUS chess)
{
  if (row == 0 || row == CHESSBOARD_DIMEN + 1)
    /* if at the first or the last row, print the coordinate with letter*/
    cout << setw(4) << ((col == 0 || col == CHESSBOARD_DIMEN + 1) ? ' ' : (char)(64 + col));
  else if (col == 0 || col == CHESSBOARD_DIMEN + 1)
    /* if at the first or the last column, print the coordinate with number*/
    cout << setw(4) << row;
  else
  {
    string c = (col == 1 ? "   " : "───");
    if (chess == EMPTY)
    {
      switch (row)
      {
        case 1:
          switch (col)
          {
            case 1:
              c += "┌"; break;
            case CHESSBOARD_DIMEN:
              c += "┐"; break;
            default:
              c += "┬";
          }
          break;
        case CHESSBOARD_DIMEN:
          switch (col)
          {
            case 1:
              c += "└"; break;
            case CHESSBOARD_DIMEN:
              c += "┘"; break;
            default:
              c += "┴";
          }
          break;
        default:
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
    }
    else
      c += (chess == BLACK) ? CHESS_BLACK : CHESS_WHITE;

    cout << c;
  }

  /* if at the last column, print \n*/
  if (col == CHESSBOARD_DIMEN + 1)
  {
    cout << "\n    ";

    /* if not at the first or last row, print │ between two row*/
    if (row > 0 && row < CHESSBOARD_DIMEN)
      for (int i = 0; i < CHESSBOARD_DIMEN; ++i)
        cout << "   │";

    cout << "\n";
  }
}

/* puts a new chess, if the point is not empty then return false*/
bool ChessBoard::play(STATUS color, int row, int col)
{
  if (point[row][col]->color != EMPTY) return false;  
  
  //records[playNo] = Point(row, col, color);
  ++playNo;
  //pointStatus[row][col] = color;
  point[row][col]->play(color, playNo);
  blackTurn = !blackTurn;

  invalidate();

  return true;
}

/* clears the whole game */
void ChessBoard::wipe(bool isInvalidate)
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      //pointStatus[r][c] = EMPTY;
      point[r][c]->reset();

  playNo = 0;
  blackTurn = true;

  if (isInvalidate)
    invalidate();
}

bool ChessBoard::isBlackTurn()
{
  return blackTurn;
}

/* search the whole board for winning conditions */
STATUS ChessBoard::judge(){

  /* index: 0→ 1↓ 2↗ 3↘*/
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  /* INclude lowerbound when searching */
  const int lowerBound[4][2] = {{0, 0}, {0, 0}, {4, 0}, {0, 0}}; 

  /* EXclude upperbound when searching */
  const int upperBound[4][2] = {
  {CHESSBOARD_DIMEN, CHESSBOARD_DIMEN - 4},
  {CHESSBOARD_DIMEN - 4, CHESSBOARD_DIMEN},
  {CHESSBOARD_DIMEN, CHESSBOARD_DIMEN - 4},
  {CHESSBOARD_DIMEN - 4, CHESSBOARD_DIMEN - 4}}; 

  STATUS targetColor[2] = {BLACK, WHITE};

  for (int color = 0; color < 2; ++color)
    /* for each direction d, start searching from lowerbound, and stop at upperBound */
    for (int d = 0; d < 4; ++d)
      for (int r = lowerBound[d][0]; r < upperBound[d][0]; ++r)
        for (int c = lowerBound[d][1]; c < upperBound[d][1]; ++c)
          for (int offset = 0; offset < 5; ++offset)
          {
            int checkRow = r + offset * dir[d][0],
              checkCol = c + offset * dir[d][1];

            if (point[checkRow][checkCol]->color != targetColor[color])
              break;

            if (offset == 4)
              return targetColor[color];
          }
  
  return EMPTY;
}

bool ChessBoard::judge(STATUS color, int row, int col)
{
  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int d = 0; d < 4; ++d)
  {
    int length = 1;

    /* from (row, col), move backward and then forward along the chosen direction*/
    /* check if the same color appears consecutively*/
    for (int move = -1; move <= 1; move += 2)
      for (int offset = 1; offset <= 4; ++offset)
      {
        int checkRow = row + dir[d][0] * move * offset,
          checkCol = col + dir[d][1] * move * offset;

        /* check if out the bound*/
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
          checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        if (point[checkRow][checkCol]->color != color)
          break;

        ++length;
        if (length == 5) return true;
      }
  }

  return false;
}