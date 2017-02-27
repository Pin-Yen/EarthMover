#ifndef STATUS_ENUM
#define STATUS_ENUM
   enum STATUS{BLACK = 0, WHITE = 1, EMPTY = 2, BOUND = 3, ANALYZE_POINT = 4, NO_MATTER = 5};
#endif

class Point
{
public:

  /* constructor */
  Point(int row, int col);

  void play(STATUS color, int playNo);

  void reset();

  void setDirStatus(int dir, int offset, STATUS* status);

  STATUS color;
  
private:

  int row, col, playNo;

  STATUS* dirStatus[4][11];

  int score;

  bool isForbidden;
};