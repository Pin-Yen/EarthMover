#include "chesstype.hpp"
#include "status.hpp"

class Point
{
public:

  /* constructor */
  Point(int row, int col);

  Point(Point* source);

  /* play at this point */
  void play(STATUS color, int playNo);

  /* reset this point */
  void reset();

  /* set the status pointer */
  /* note: to finish initialize this point, shound call this to write all status pointer */
  void setDirStatus(int dir, int offset, STATUS* status) { dirStatus[dir][offset] = status; }

  int Point::getRow() { return row; }

  int Point::getColumn() { return col; }

  int Point::getPlayNo() { return playNo; }

  void Point::getScore(int* dest) { dest[0] = score[0]; dest[1] = score[1]; }

  /* point's status, the target of the other point's STATUS array pointer*/
  STATUS status;

  ChessType* type[4][2];
private:

  int row, col, playNo;

  /* STATUS array pointer, this will point at other point's Status color */
  /* index: 0→ 1↓ 2↗ 3↘ */
  STATUS* dirStatus[4][8];

  int score[2];
};