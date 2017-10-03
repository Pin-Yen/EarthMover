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

void DisplayBoard::invalidate() const {
  for (int r = 0; r < DIMEN + 2; ++r) {
    for (int c = 0; c < DIMEN + 2; ++c) {
      if (r > 0 && r < DIMEN + 1 &&
          c > 0 && c < DIMEN + 1) {
        printBoard(r, c, point_[r - 1][c - 1]);
      } else {
        printBoard(r, c, 0);
      }
    }
  }
}

void DisplayBoard::getInput(int *row, int *col) const {
  do {
    std::cout << "enter the coordinate of next move (A1 ~ "
              << static_cast<char>('A' + DIMEN - 1)
              << DIMEN << ") : ";
    std::string input;
    std::cin >> input;

    int inputLength = input.length();

    bool validInput = false;
    if (inputLength == 2 || inputLength == 3) {
      validInput = true;

      // get column
      *col = input[0];
      if (*col >= 'A' && *col < 'A' + DIMEN) {
        *col -= 'A';
      } else if (*col >= 'a' && *col < 'a' + DIMEN) {
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

      if (*row >= 1 && *row <= DIMEN) {
        --(*row);
      } else {
        validInput = false;
      }
    }

    if (validInput) break;

    std::cout << "Invalid input\n";

  } while (true);
}

void DisplayBoard::printBoard(int row, int col, int color) const {
  if (row == 0 || row == DIMEN + 1) {
    // if at the first or the last row, print the coordinate with letter
    std::cout << std::setw(4)
              << ((col == 0 || col == DIMEN + 1) ?
                     ' ' : static_cast<char>(64 + col));
  } else if (col == 0 || col == DIMEN + 1) {
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
    } else if (row == 1 || row == DIMEN) {
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
            case DIMEN:
              s += "╗"; break;
            default:
              s += "╤";
          }
          break;
        case DIMEN:
          switch (col) {
            case 1:
              s += "╚"; break;
            case DIMEN:
              s += "╝"; break;
            default:
              s += "╧";
          }
          break;
        default:
          switch (col) {
            case 1:
              s += "╟"; break;
            case DIMEN:
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
  if (col == DIMEN + 1) {
    std::string s = "\n    ";

    // if not at the first or last row, print │ between two row
    if (row > 0 && row < DIMEN) {
      for (int i = 0; i < DIMEN; ++i)
        s += ((i == 0 || i == DIMEN - 1) ? "   ║" : "   │");
    }

    s += "\n";
    std::cout << s;
  }
}

// puts a new chess, if the point is not empty then return false
bool DisplayBoard::play(int row, int col) {
  if (outOfBound(row, col)) return false;
  if (point_[row][col] != 0) return false;

  ++playNo_;

  int selfColor = playNo_ & 1 ? -1 : 1;
  int oppColor = selfColor == -1 ? 1 : -1;
  point_[row][col] = selfColor;
  int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int i = 0; i < 4; ++i) {
    int checkRow = row + dir[i][0],
        checkCol = col + dir[i][1];
    if (outOfBound(checkRow, checkCol))
      continue;
    if (point_[checkRow][checkCol] == oppColor) {
      removeIfDead(checkRow, checkCol, oppColor);
    }
  }

  invalidate();

  return true;
}

void DisplayBoard::removeIfDead(int row, int col, int color) {
  std::set<int> group;
  if (noLiberty(row, col, color, &group)) {
    for (auto& pos : group) {
      point_[pos / DIMEN][pos % DIMEN] = 0;
    }
  }
}

bool DisplayBoard::noLiberty(int row, int col, int color,
                             std::set<int> *group) const {
  group->insert(row * DIMEN + col);

  // Calculate opponent's liberty around the playing point
  int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int i = 0; i < 4; ++i) {
    int checkRow = row + dir[i][0],
        checkCol = col + dir[i][1];
    if (outOfBound(checkRow, checkCol))
      continue;

    // Calculate liberty, and remove chess if dead.
    if (point_[checkRow][checkCol] == 0) {
      return false;
    } else if (point_[checkRow][checkCol] == color) {
      if (group->find(checkRow * DIMEN + checkCol) != group->end()) {
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
  for (int r = 0; r < DIMEN; ++r)
    for (int c = 0; c < DIMEN; ++c)
      point_[r][c] = 0;

  playNo_ = 0;

  invalidate();
}
