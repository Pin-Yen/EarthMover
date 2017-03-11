#include "displayboard.hpp"
#include <iomanip>
#include <string>
#include <iostream>

DisplayBoard::DisplayBoard()
{
  wipe();
}

/* prints the current chesssboard */
void DisplayBoard::invalidate()
{
  for (int r = 0; r < CHESSBOARD_DIMEN + 2; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN + 2; ++c)
    {
      if (r > 0 && r < CHESSBOARD_DIMEN + 1 && c > 0 && c < CHESSBOARD_DIMEN + 1)
        printBoard(r, c, point[r - 1][c - 1]);
      else
        printBoard(r, c, 0);
    }
}

void DisplayBoard::getInput(int &row, int &col)
{
  while (true)
  {
    std::cout << "enter the coordinate of next move (A1 ~ " 
      << (char)('A' + CHESSBOARD_DIMEN - 1) << CHESSBOARD_DIMEN << ") : ";
    std::string input;
    std::cin >> input;

    int n = input.length();

    if (n == 2 || n == 3)
    {
      bool validColumn = false, validRow = false;

      /* get column*/
      col = input[0];
      if (col >= 'A' && col <= 'A' + CHESSBOARD_DIMEN)
      {
        col -= 'A';
        validColumn = true;
      }
      else if (col >= 'a' && col <= 'a' + CHESSBOARD_DIMEN)
      {
        col -= 'a';
        validColumn = true;
      }

      /* get row*/
      bool isNumber = true;
      for (int i = 1; i < n; ++i)
        if (!isdigit(input[i]))
        {
          isNumber = false; break;
        }

      if (isNumber)
      {
        row = std::stoi(input.substr(1, n - 1));
        if (row >= 1 && row <= CHESSBOARD_DIMEN)
        {
          row--;
          validRow = true;
        }
      }   

      if (validRow && validColumn) break;  
    }

    std::cout << "Invalid input\n";
  }
}

/* print a part of the board*/
void DisplayBoard::printBoard(int row, int col, int color)
{
  if (row == 0 || row == CHESSBOARD_DIMEN + 1)
    /* if at the first or the last row, print the coordinate with letter */
    std::cout << std::setw(4) << 
      ((col == 0 || col == CHESSBOARD_DIMEN + 1) ? ' ' : (char)(64 + col));
  else if (col == 0 || col == CHESSBOARD_DIMEN + 1)
    /* if at the first or the last column, print the coordinate with number */
    std::cout << std::setw(4) << row;
  else
  {
    std::string s;
    if (col == 1)
      s = "   ";
    else if (row == 1 || row == CHESSBOARD_DIMEN)
      s = "═══";
    else if ((row == 4 && col == 4) || 
      (row == 4 && col == CHESSBOARD_DIMEN - 3) ||
      (row == CHESSBOARD_DIMEN - 3 && col == 4) || 
      (row == CHESSBOARD_DIMEN - 3 && col == CHESSBOARD_DIMEN - 3) ||
      (row == CHESSBOARD_DIMEN / 2 + 1 && col == CHESSBOARD_DIMEN / 2 + 1)) 
      s = "──╼";
    else if ((row == 4 && col == 5) || 
      (row == 4 && col == CHESSBOARD_DIMEN - 2) ||
      (row == CHESSBOARD_DIMEN - 3 && col == 5) || 
      (row == CHESSBOARD_DIMEN - 3 && col == CHESSBOARD_DIMEN - 2) ||
      (row == CHESSBOARD_DIMEN / 2 + 1 && col == CHESSBOARD_DIMEN / 2 + 2)) 
      s = "╾──";
    else
      s = "───";

    if (color == 0)
    {
      switch (row)
      {
        case 1:
          switch (col)
          {
            case 1:
              s += "╔"; break;
            case CHESSBOARD_DIMEN:
              s += "╗"; break;
            default:
              s += "╤";
          }
          break;
        case CHESSBOARD_DIMEN:
          switch (col)
          {
            case 1:
              s += "╚"; break;
            case CHESSBOARD_DIMEN:
              s += "╝"; break;
            default:
              s += "╧";
          }
          break;
        default:
          switch (col)
          {
            case 1:
              s += "╟"; break;
            case CHESSBOARD_DIMEN:
              s += "╢"; break;
            default:
              s += (((row == 4 && col == 4) || 
                (row == 4 && col == CHESSBOARD_DIMEN - 3) ||
                (row == CHESSBOARD_DIMEN - 3 && col == 4) || 
                (row == CHESSBOARD_DIMEN - 3 && col == CHESSBOARD_DIMEN - 3) ||
                (row == CHESSBOARD_DIMEN / 2 + 1 && col == CHESSBOARD_DIMEN / 2 + 1)) 
                ? "╋" : "┼");
          }
      }
    }
    else
      s += (color & 1) ? CHESS_BLACK : CHESS_WHITE;

    std::cout << s;
  }

  /* if at the last column, print \n */
  if (col == CHESSBOARD_DIMEN + 1)
  {
    std::string s = "\n    ";

    /* if not at the first or last row, print │ between two row */
    if (row > 0 && row < CHESSBOARD_DIMEN)
      for (int i = 0; i < CHESSBOARD_DIMEN; ++i)
        s += ((i == 0 || i == CHESSBOARD_DIMEN - 1) ? "   ║" : "   │");

    s += "\n";
    std::cout << s;
  }
}

/* puts a new chess, if the point is not empty then return false */
bool DisplayBoard::play(int row, int col)
{
  if (point[row][col] != 0) return false;  
  
  ++playNo;

  point[row][col] = playNo;

  invalidate();

  return true;
}

/* clears the whole game */
void DisplayBoard::wipe()
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = 0;

  playNo = 0;

  invalidate();
}

/* search the area surrounding (row, col) for winning conditions */
bool DisplayBoard::judge(int row, int col)
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

        int checkPoint = point[checkRow][checkCol];

        if (checkPoint == 0 || checkPoint & 1 == playNo & 1)
          break;

        ++length;
        if (length == 5) return true;
      }
  }

  return false;
}