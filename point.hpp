#include "chesstype.hpp"

#ifndef STATUS_ENUM
#define STATUS_ENUM
   enum STATUS{BLACK = 0, WHITE = 1, EMPTY = 2, BOUND = 3, ANALYZE_POINT = 4, NO_MATTER = 5};
#endif

class Point
{
public:

  /* constructor */
  Point(int row, int col);

  /* play at this point */
  void play(STATUS color, int playNo);

  /* reset this point */
  void reset();

  /* set the status pointer */
  /* note: to finish initialize this point, shound call this to write all status pointer */
  void setDirStatus(int dir, int offset, STATUS* status);

  /* point's color, the target of the other point's STATUS array pointer*/
  STATUS color;
  
private:

  int row, col, playNo;

  /* STATUS array pointer, this will point at other point's Status color */
  /* index: 0→ 1↓ 2↗ 3↘ */
  STATUS* dirStatus[4][11];

  ChessType* type[4][2];

  int score[2];

  bool isForbidden;
};