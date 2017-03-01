#include "chessboard.hpp"
#include <iomanip>
#include <iostream>

using namespace std;

ChessBoard::ChessBoard()
{
  /* initialize point array */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(r, c);

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
    {

      /* set each poit's status array pointer*/
      for (int d = 0; d < 4; ++d)
        for (int offset = -5; offset < 6; ++offset)
        {
          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          STATUS* status;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          {
            /* if out of bound, point to bound */
            STATUS s = BOUND; status = &s;
          }
          else
            status = &(point[r][c]->status);

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
        printBoard(r, c, point[r - 1][c - 1]->status);
      else
        printBoard(r, c, EMPTY);
    }
}

/* print a part of the board*/
void ChessBoard::printBoard(int row, int col, STATUS status)
{
  if (row == 0 || row == CHESSBOARD_DIMEN + 1)
    /* if at the first or the last row, print the coordinate with letter */
    cout << setw(4) << ((col == 0 || col == CHESSBOARD_DIMEN + 1) ? ' ' : (char)(64 + col));
  else if (col == 0 || col == CHESSBOARD_DIMEN + 1)
    /* if at the first or the last column, print the coordinate with number */
    cout << setw(4) << row;
  else
  {
    string c;
    if (col == 1)
      c = "   ";
    else if (row == 1 || row == CHESSBOARD_DIMEN)
      c = "═══";
    else
      c = "───";

    if (status == EMPTY)
    {
      switch (row)
      {
        case 1:
          switch (col)
          {
            case 1:
              c += "╔"; break;
            case CHESSBOARD_DIMEN:
              c += "╗"; break;
            default:
              c += "╤";
          }
          break;
        case CHESSBOARD_DIMEN:
          switch (col)
          {
            case 1:
              c += "╚"; break;
            case CHESSBOARD_DIMEN:
              c += "╝"; break;
            default:
              c += "╧";
          }
          break;
        default:
          switch (col)
          {
            case 1:
              c += "╟"; break;
            case CHESSBOARD_DIMEN:
              c += "╢"; break;
            default:
              c += "┼";
          }
      }
    }
    else
      c += (status == BLACK) ? CHESS_BLACK : CHESS_WHITE;

    cout << c;
  }

  /* if at the last column, print \n */
  if (col == CHESSBOARD_DIMEN + 1)
  {
    cout << "\n    ";

    /* if not at the first or last row, print │ between two row */
    if (row > 0 && row < CHESSBOARD_DIMEN)
      for (int i = 0; i < CHESSBOARD_DIMEN; ++i)
        cout << "   " << ((i == 0 || i == CHESSBOARD_DIMEN - 1) ? "║" : "│");

    cout << "\n";
  }
}

/* puts a new chess, if the point is not empty then return false */
bool ChessBoard::play(STATUS color, int row, int col)
{
  if (point[row][col]->status != EMPTY) return false;  
  
  ++playNo;

  point[row][col]->play(color, playNo);

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int d = 0; d < 4; ++d)
  {
    int length = 1;

    for (int move = -1; move <= 1; move = 2)
    {
      bool block[2] = {false, false};

      for (int offset = 1; offset <= 5; ++offset)
      {
        int checkRow = row + dir[d][0] * move * offset,
          checkCol = col + dir[d][1] * move * offset;

        /* check if out the bound */
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
          checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        if (point[checkRow][checkCol]->status != EMPTY)
        {
          block[point[checkRow][checkCol]->status] = true;
          if (block[0] & block[1]) break;
          continue;
        }

        //evaluate(point[checkRow][checkCol]->type, point[checkRow][checkCol]->status,
        //  d, checkForbidden);
      }
    }
  }

  blackTurn = !blackTurn;

  invalidate();

  return true;
}

/* clears the whole game */
void ChessBoard::wipe(bool isInvalidate)
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
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

/* search the area surrounding (row, col) for winning conditions */
bool ChessBoard::judge(STATUS color, int row, int col)
{
  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int d = 0; d < 4; ++d)
  {
    int length = 1;

    /* from (row, col), move backward and then forward along the chosen direction */
    /* check if the same color appears consecutively */
    for (int move = -1; move <= 1; move += 2)
      for (int offset = 1; offset <= 4; ++offset)
      {
        int checkRow = row + dir[d][0] * move * offset,
          checkCol = col + dir[d][1] * move * offset;

        /* check if out the bound */
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
          checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        if (point[checkRow][checkCol]->status != color)
          break;

        ++length;
        if (length == 5) return true;
      }
  }

  return false;
}