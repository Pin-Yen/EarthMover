#include "virtualboardgo.h"

#include <set>

VirtualBoardGo::VirtualBoardGo() : playNo_(0), koPosition_(0) {}

GameStatus VirtualBoardGo::play(int index) {
  ++playNo_;

  StoneStatus selColor = ((playNo_ & 1) ? BLACK : WHITE),
              oppColor = ((playNo_ & 1) ? WHITE : BLACK);

  point_[index].setStatus(selColor);

  const int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
  int row = index / DIMEN, col = index % DIMEN;
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
