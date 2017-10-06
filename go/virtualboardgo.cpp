#include "virtualboardgo.h"

#include <set>

VirtualBoardGo::VirtualBoardGo() : playNo_(0), koPoint_(0) {}

GameStatus VirtualBoardGo::play(int index) {
  ++playNo_;

  StoneStatus selColor = ((playNo_ & 1) ? BLACK : WHITE),
              oppColor = ((playNo_ & 1) ? WHITE : BLACK);

  const int row = index / DIMEN, col = index % DIMEN;

  point_[index].setStatus(selColor);
  koPoint_ = findKo();

  const int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int d = 0; d < 4; ++d) {
    const int checkRow = row + dir[d][0],
              checkCol = col + dir[d][1];

    if (outOfBound(checkRow, checkCol)) continue;

    const int checkIndex = checkRow * DIMEN + checkCol;

    if (point_[checkIndex].status() == oppColor) {
      removeIfDead(checkRow, checkCol, oppColor);
    }
  }
}

int VirtualBoardGo::findKo(int row, int col,
                           StoneStatus selfColor, StoneStatus oppColor) const {
  int koPoint = -1;

  const int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  for (int d = 0; d < 4; ++d) {
    const int checkRow = row + dir[i][0],
              checkCol = col + dir[i][1];
    if (outOfBound(checkRow, checkCol)) continue;

    if (point_[checkRow][checkCol].status() != oppColor) {
      return -1;
    } else {
      std::set<int> group;
      if (noLiberty(checkRow, checkCol, oppColor, group)) {
        if (group.size() == 1) {
          if (koPoint != -1) {
            return -1;
          } else {
            koPoint = checkRow * DIMEN + checkCol;
          }
        }
      }
    }
  }
  return koPoint;
}

void VirtualBoardGo::removeIfDead(int row, int col, StoneStatus color) {
  std::set<int> group;
  if (noLiberty(row, col, color, &group)) {
    for (const int& index : group) {
      point_[index].setStatus(EMPTY);
    }
  }
}

bool VirtualBoardGo::noLiberty(int row, int col, StoneStatus color,
                               std::set<int> *group) const {
  group->insert(row * DIMEN + col);

  const int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

  for (int d = 0; d < 4; ++d) {
    const int checkRow = row + dir[d][0],
              checkCol = col + dir[d][1];

    if (outOfBound(checkRow, checkCol)) continue;

    const int checkIndex = checkRow * DIMEN + checkCol;
    StoneStatus status = point_[checkIndex].status();
    if (status == EMPTY) {
      return false;
    } else if (status == color) {
      if (group->find(checkIndex) != group->end()) continue;
      if (!noLiberty(checkRow, checkCol, color, group)) return false;
    }
  }
  return true;
}

bool VirtualBoardGo::isSuicideMove(
    int index, StoneStatus selfColor, StoneStatus oppColor) {
  const int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  const int row = index / DIMEN, col = index % DIMEN;
  for (int d = 0; d < 4; ++d) {
    const int checkRow = row + dir[d][0],
              checkCol = col + dir[d][1];
    if (outOfBound(checkRow, checkCol)) continue;

    StoneStatus status = point_[checkIndex].status();
    if (status == EMPTY) {
      return false;
    } else if (status == oppColor) {
      //TODO
      //point_[index] = selfColor;
      std::set<int> group;
      bool oppNoLiberty = noLiberty(checkRow, checkCol, oppColor, group);
      //point_[index] = EMPTY;
      if (oppNoLiberty) return false;
    } else {
      //TODO
      //this.board[row][col] = oppColor;
      std::set<int> group;
      bool selfNoLiberty = noLiberty(checkRow, checkCol, selfColor, group);
      //this.board[row][col] = this.EMPTY;
      if (!selfNoLiberty) return false;
    }
  }
  return true;
}
