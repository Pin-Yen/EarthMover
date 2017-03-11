#include "evaluator.hpp"

void Evaluator::evaluate_type(ChessType* type[2], STATUS *status)
{
  TypeTree::classify(status, type);
}

/* score[0]:black's total score,[1]:white's */
void Evaluator::evaluate_score(ChessType* type[4][2], int *score)
{
  /* count[color][length][LorD]
   * color: 0 for BLACK, 1 for WHITE
   * length: means the length of the type, should be 0~5.
   * LorD: 0 for LIVE, 1 for DEAD 
   * e.g count[1][4][0] means the number of white dead fours */
  int count[2][6][2] = {{{0}}};

  /* count the types in 4 directions */
  for (int color = 0; color < 2; ++color)
    for (int d = 0; d < 4; ++d)
      ++(count[color][type[d][color]->length][type[d][color]->life]);

  /* [0] attack, [1] defense*/
  const int SCORE_5[2] = {1000000, 80000};
  const int SCORE_LIVE4[2] = {8000, 2000};
  const int SCORE_DOUBLE4[2] = {8000, 2000};
  const int SCORE_DEAD4LIVE3[2] = {6000, 1600};

  const int SCORE_DOUBLELIVE3[2] = {1000, 240};
  const int SCORE_DEAD4[2] = {200, 120};
  const int SCORE_LIVE3[2] = {200, 100};
  const int SCORE_DEAD3[2] = {64, 36};
  const int SCORE_LIVE2[2] = {60, 36};
  const int SCORE_DEAD2[2] = {25, 12};
  const int SCORE_LIVE1[2] = {20, 10};
  const int SCORE_DEAD1[2] = {6, 2};

  /*these const are for indexing purposes, enhancing the readibility of the code. */
  const int ATTACK = 0;
  const int DEFENSE = 1;
  const int BLACK = 0;
  const int WHITE = 1;
  const int LIVE = 1;
  const int DEAD = 0;

  score[BLACK] = 0; score[WHITE] = 0;

  /* calculate score */
  for (int selfColor = BLACK, opponentColor = WHITE, i = 0; i < 2; 
    selfColor = WHITE, opponentColor = BLACK, ++i)
  {
    /* self 5 */
    if (count[selfColor][5][0] > 0)
      score[selfColor] = SCORE_5[ATTACK];
    /* opponent 5 */
    else if (count[opponentColor][5][0] > 0)
      score[selfColor] = SCORE_5[DEFENSE];

    /* self l4 */
    else if (count[selfColor][4][LIVE] > 0)
      score[selfColor] = SCORE_LIVE4[ATTACK];
    /* opponent l4 */
    else if (count[opponentColor][4][LIVE] > 0)
      score[selfColor] = SCORE_LIVE4[DEFENSE];

    /* self muliy-4 */
    else if (count[selfColor][4][DEAD] >= 2)
      score[selfColor] = SCORE_DOUBLE4[ATTACK];
    /* opponent muliy-4 */
    else if (count[opponentColor][4][DEAD] >= 2)
      score[selfColor] = SCORE_DOUBLE4[DEFENSE];

    /* self d4-l3 */
    else if (count[selfColor][4][DEAD] > 0 && count[selfColor][3][LIVE] > 0)
      score[selfColor] = SCORE_DEAD4LIVE3[ATTACK];
    /* opponent d4-l3 */
    else if (count[opponentColor][4][DEAD] > 0 && count[opponentColor][3][LIVE] > 0)
      score[selfColor] = SCORE_DEAD4LIVE3[DEFENSE];
    else
    {
      /* self multi-3 */
      if (count[selfColor][3][LIVE] >= 2)
        score[selfColor] += SCORE_DOUBLELIVE3[ATTACK];
      /* opponent multi-3 */
      if (count[opponentColor][3][LIVE] >= 2)
        score[selfColor] += SCORE_DOUBLELIVE3[DEFENSE];

      /* self dead 4 */
      if (count[selfColor][4][DEAD])
        score[selfColor] += SCORE_DEAD4[ATTACK];
      /* opponent dead4 */
      if (count[opponentColor][4][DEAD])
        score[selfColor] += SCORE_DEAD4[DEFENSE];

      /* self live 3 */
      if (count[selfColor][3][LIVE] == 1)
        score[selfColor] += SCORE_LIVE3[ATTACK];
      /* opponent live 3 */
      if (count[opponentColor][3][LIVE] == 1)
        score[selfColor] += SCORE_LIVE3[DEFENSE];
      /* self dead 3 */
      if (count[selfColor][3][DEAD])
        score[selfColor] += SCORE_DEAD3[ATTACK] * count[selfColor][3][DEAD];        
      /* opponent dead 3 */
      if (count[opponentColor][3][DEAD])
        score[selfColor] += SCORE_DEAD3[DEFENSE] * count[opponentColor][3][DEAD];

      /* self live 2 */
      if (count[selfColor][2][LIVE])
        score[selfColor] += SCORE_LIVE2[ATTACK] * count[selfColor][2][LIVE];
      /* opponent live 2 */
      if (count[opponentColor][2][LIVE])
        score[selfColor] += SCORE_LIVE2[DEFENSE] * count[opponentColor][2][LIVE];
      /* self dead 2 */
      if (count[selfColor][2][DEAD])
        score[selfColor] += SCORE_DEAD2[ATTACK] * count[selfColor][2][DEAD];        
      /* opponent dead 2 */
      if (count[opponentColor][2][DEAD])
        score[selfColor] += SCORE_DEAD2[DEFENSE] * count[opponentColor][2][DEAD];

      /* self live 1 */
      if (count[selfColor][1][LIVE])
        score[selfColor] += SCORE_LIVE1[ATTACK] * count[selfColor][1][LIVE];
      /* opponent live 1 */
      if (count[opponentColor][1][LIVE])
        score[selfColor] += SCORE_LIVE1[DEFENSE] * count[opponentColor][1][LIVE];
      /* self dead 1 */
      if (count[selfColor][1][DEAD])
        score[selfColor] += SCORE_DEAD1[ATTACK] * count[selfColor][1][DEAD];        
      /* opponent dead 1*/
      if (count[opponentColor][1][DEAD])
        score[selfColor] += SCORE_DEAD1[DEFENSE] * count[opponentColor][1][DEAD];
    }
  }
}