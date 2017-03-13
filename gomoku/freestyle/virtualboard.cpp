#include "virtualboard.hpp"

VirtualBoard::VirtualBoard()
{
  /* initialize point array */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(r, c);

  /* initialize score*/
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      for (int color = 0; color < 2; ++color)
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
            /* if out of bound, set pointer value to bound */
            status = new STATUS(BOUND);
          else
            status = &(point[checkRow][checkCol]->status);

          point[r][c]->setDirStatus(d, index, status);
          ++index;
        }

  playNo = 0;

  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
    {
      for (int d = 0; d < 4; ++d)
      {
        STATUS status[8]; point[r][c]->getDirStatusArray(d, status);

        Evaluator::evaluate_type(point[r][c]->type[d], status);
      }
      Evaluator::evaluate_score(point[r][c]->type, point[r][c]->score);
    }
}

VirtualBoard::VirtualBoard(VirtualBoard* source)
{
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      point[r][c] = new Point(source->point[r][c]);

  /* copy score */
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
      for (int color = 0; color < 2; ++color)
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
            /* if out of bound, set pointer value to bound */
            status = new STATUS(BOUND);
          else
            status = &(point[checkRow][checkCol]->status);

          point[r][c]->setDirStatus(d, index, status);
          ++index;
        }

  playNo = source->playNo;
}

/* get the highest score position*/
void VirtualBoard::getHSP(int &row, int &col, bool black)
{
  int max = -1000, same = 1;
  for (int r = 0; r < CHESSBOARD_DIMEN; ++r)
    for (int c = 0; c < CHESSBOARD_DIMEN; ++c)
    {
      if (point[r][c]->status != EMPTY) continue;

      if (point[r][c]->score[black] > max)
      {
        same = 1;

        max = point[r][c]->score[black];
        row = r; col = c;
      }
      else if (point[r][c]->score[black] == max)
      {
        ++same;
        if (((double)rand() / RAND_MAX) <= (1. / same))
        {
          row = r; col = c;
        }
      }
    }
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

        STATUS status[8]; point[checkRow][checkCol]->getDirStatusArray(d, status);

        Evaluator::evaluate_type(point[checkRow][checkCol]->type[d], status);

        Evaluator::evaluate_score(point[checkRow][checkCol]->type, 
          point[checkRow][checkCol]->score);
      }
    }
}