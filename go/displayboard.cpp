#include "displayboard.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <string>
#include <iostream>
#include <vector>

DisplayBoard::DisplayBoard() {
  clear();
}

// prints the current chesssboard
void DisplayBoard::invalidate() const {
  for (int r = 0; r < CHESSBOARD_DIMEN + 2; ++r) {
    for (int c = 0; c < CHESSBOARD_DIMEN + 2; ++c) {
      if (r > 0 && r < CHESSBOARD_DIMEN + 1 &&
          c > 0 && c < CHESSBOARD_DIMEN + 1) {
        printBoard(r, c, board[r - 1][c - 1]);
      } else {
        printBoard(r, c, 0);
      }
    }
  }
}

void DisplayBoard::getInput(int *row, int *col) const {
  do {
    std::cout << "enter the coordinate of next move (A1 ~ "
              << static_cast<char>('A' + CHESSBOARD_DIMEN - 1)
              << CHESSBOARD_DIMEN << ") : ";
    std::string input;
    std::cin >> input;

    int inputLength = input.length();

    bool validInput = false;
    if (inputLength == 2 || inputLength == 3) {
      validInput = true;

      // get column
      *col = input[0];
      if (*col >= 'A' && *col < 'A' + CHESSBOARD_DIMEN) {
        *col -= 'A';
      } else if (*col >= 'a' && *col < 'a' + CHESSBOARD_DIMEN) {
        *col -= 'a';
      } else {
        validInput = false;
      }

      // get row
      bool isNumber = true;
      for (int i = 1; i < inputLength; ++i) {
        if (!isdigit(input[i])) {
          isNumber = false;
          break;
        }
      }

      if (!isNumber) validInput = false;


      *row = std::stoi(input.substr(1, inputLength - 1));

      if (*row >= 1 && *row <= CHESSBOARD_DIMEN) {
        --(*row);
      } else {
        validInput = false;
      }
    }

    if (validInput) break;

    std::cout << "Invalid input\n";

  } while (true);
}

// print a part of the board
void DisplayBoard::printBoard(int row, int col, int color) const {
  if (row == 0 || row == CHESSBOARD_DIMEN + 1) {
    // if at the first or the last row, print the coordinate with letter
    std::cout << std::setw(4)
              << ((col == 0 || col == CHESSBOARD_DIMEN + 1) ?
                     ' ' : static_cast<char>(64 + col));
  } else if (col == 0 || col == CHESSBOARD_DIMEN + 1) {
    // if at the first or the last column, print the coordinate with number
    if (col == 0) {
      std::cout << std::setw(4) << row;
    } else {
      std::cout << "   " << row;
    }
  } else {
    std::string s;
    if (col == 1) {
      s = "   ";
    } else if (row == 1 || row == CHESSBOARD_DIMEN) {
      s = "═══";
    } else if (isStarPoint(row, col)) {
      s = "──╼";
    } else if (isStarPoint(row, col - 1)) {
      s = "╾──";
    } else {
      s = "───";
    }

    if (color == 0) {
      switch (row) {
        case 1:
          switch (col) {
            case 1:
              s += "╔"; break;
            case CHESSBOARD_DIMEN:
              s += "╗"; break;
            default:
              s += "╤";
          }
          break;
        case CHESSBOARD_DIMEN:
          switch (col) {
            case 1:
              s += "╚"; break;
            case CHESSBOARD_DIMEN:
              s += "╝"; break;
            default:
              s += "╧";
          }
          break;
        default:
          switch (col) {
            case 1:
              s += "╟"; break;
            case CHESSBOARD_DIMEN:
              s += "╢"; break;
            default:
              s += (isStarPoint(row, col) ? "╋" : "┼");
          }
      }
    } else {
      s += (color == -1) ? CHESS_BLACK : CHESS_WHITE;
    }

    std::cout << s;
  }

  // if at the last column, print \n
  if (col == CHESSBOARD_DIMEN + 1) {
    std::string s = "\n    ";

    // if not at the first or last row, print │ between two row
    if (row > 0 && row < CHESSBOARD_DIMEN) {
      for (int i = 0; i < CHESSBOARD_DIMEN; ++i)
        s += ((i == 0 || i == CHESSBOARD_DIMEN - 1) ? "   ║" : "   │");
    }

    s += "\n";
    std::cout << s;
  }
}

// puts a new chess, if the point is not empty then return false
bool DisplayBoard::play(int row, int col) {
  if (outOfBound(row, col)) return false;
  if (board[row][col] != 0) return false;

  ++playNo_;

  int selfColor = playNo_ & 1 ? -1 : 1;
  int oppColor = selfColor == -1 ? 1 : -1;
  board[row][col] = selfColor;
  int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int i = 0; i < 4; ++i) {
    int checkRow = row + dir[i][0],
        checkCol = col + dir[i][1];
    if (outOfBound(checkRow, checkCol))
      continue;
    if (board[checkRow][checkCol] == oppColor) {
      removeIfDead(checkRow, checkCol, oppColor);
    }
  }

  invalidate();

  return true;
}

void DisplayBoard::removeIfDead(int row, int col, int color) {
  std::vector<std::array<int, 2>> group;
  if (noLiberty(row, col, color, &group)) {
    for (auto& pos : group) {
      board[pos[0]][pos[1]] = 0;
    }
  }
}

bool DisplayBoard::noLiberty(int row, int col,
                             int color, std::vector<std::array<int, 2>> *group) {
  group->push_back({row, col});

  // Calculate opponent's liberty around the playing point
  int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int i = 0; i < 4; ++i) {
    int checkRow = row + dir[i][0],
        checkCol = col + dir[i][1];
    if (outOfBound(checkRow, checkCol))
      continue;

    // Calculate liberty, and remove chess if dead.
    if (board[checkRow][checkCol] == 0) {
      return false;
    } else if (board[checkRow][checkCol] == color) {
      std::array<int, 2> pos = {checkRow, checkCol};
      if (std::find(group->begin(), group->end(), pos) !=
          group->end()) {
        continue;
      }

      if (!noLiberty(checkRow, checkCol, color, group)) {
        return false;
      }
    }
  }
  return true;
}

// clears the whole game
void DisplayBoard::clear() {
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      board[r][c] = 0;

  playNo_ = 0;

  invalidate();
}
