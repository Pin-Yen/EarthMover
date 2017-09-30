#include "virtualboardgo.h"

#include <vector>

GameStatus VirtualBoardGo::play(int index) {

}

bool VirtualBoardGo::noLiberty(int row, int col, StoneStatus color,
                               std::vector<int> *group) const {
  group.push_back(row * DIMEN + col);

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
      // TODO: if checkIndex in group:
      if (true) continue;
      if (!noLiberty(checkRow, checkCol, color, group)) return false;
    }
  }
  return true;
}
