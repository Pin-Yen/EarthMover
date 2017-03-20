#ifndef VIRTUAL_BOARD_POINT_H
#define VIRTUAL_BOARD_POINT_H

class VirtualBoard::Point {
 public:
  /* constructor */
  Point() { status = EMPTY; };

  Point(Point* source);

  /* play at this point */
  void play(STATUS status) { this->status = status; }

  /* set the status pointer */
  /* note: to finish initialize this point, shound call this to write all status pointer */
  void setDirStatus(int dir, int offset, STATUS* status) { dirStatus[dir][offset] = status; }

  void getDirStatusArray(int dir, STATUS* dest);

  /* point's status, the target of the other point's STATUS array pointer*/
  STATUS status;

  ChessType* type[4][2];
 private:
  /* STATUS array pointer, this will point at other point's Status color */
  /* index: 0→ 1↓ 2↗ 3↘ */
  STATUS* dirStatus[4][8];
};

#endif