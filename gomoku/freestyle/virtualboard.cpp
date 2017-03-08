#include "vitualboard.hpp"

VirtualBoard::VirtualBoard()
{
  /* initialize point array */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(r, c);

  /* initialize score*/
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      for(int color = 0; color < 2; ++color)
        score[r][c][color] = 0;

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* set each poit's status array pointer*/
      for (int d = 0; d < 4; ++d)
        for (int offset = -4, index = 0; offset <= 4; ++offset)
        {
          if (offset == 0) continue;

          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          STATUS* status;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          {
            /* if out of bound, point to bound */
            STATUS s = BOUND; status = &s;
          }
          else
            status = &(point[r][c]->status);

          point[r][c]->setDirStatus(d, index, status);
          ++index
        }


  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c]->reset();

  playNo = 0;

}

VirtualBoard::VirtualBoard(VirtualBoard* source)
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(source->point[r][c]);

  /* copy score */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      for(int color = 0; color < 2; ++color)
        score[r][c][color] = source->score[r][c][color];

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      /* set each point's status array pointer*/
      for (int d = 0; d < 4; ++d)
        for (int offset = -4, index = 0; offset <= 4; ++offset)
        {
          if (offset == 0) continue;

          int checkRow = r + dir[d][0] * offset,
            checkCol = c + dir[d][1] * offset;

          STATUS* status;

          if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
            checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          {
            /* if out of bound, point to bound */
            STATUS s = BOUND; status = &s;
          }
          else
            status = &(point[r][c]->status);

          point[r][c]->setDirStatus(d, index, status);
          ++index
        }

  playNo = source->playNo;
}

/* puts a new chess */
void VirtualBoard::play(int row, int col)
{
  ++playNo;

  STATUS color = ((playNo & 1) ? BLACK : WHITE);

  point[row][col]->play(color, playNo);

  /* index: 0→ 1↓ 2↗ 3↘ */
  const int dir[4][2] = {{0, 1}, {1, 0}, {-1, 1}, {1, 1}};

  for (int d = 0; d < 4; ++d)
  {
    int length = 1;

    for (int move = -1; move <= 1; move += 2)
    {
      bool block[2] = {false, false};

      for (int offset = 1; offset <= 5; ++offset)
      {
        int checkRow = row + dir[d][0] * move * offset,
          checkCol = col + dir[d][1] * move * offset;

        /* check if out the bound */
        if (checkRow < 0 || checkRow >= CHESSBOARD_DIMEN || 
          checkCol < 0 || checkCol >= CHESSBOARD_DIMEN)
          break;

        if (point[checkRow][checkCol]->status != EMPTY)
        {
          block[point[checkRow][checkCol]->status] = true;
          if (block[0] & block[1]) break;
          continue;
        }

        evaluate(point[checkRow][checkCol]->type, point[checkRow][checkCol]->status,
         d, score[checkRow][checkCol]);
      }
    }
  }
}