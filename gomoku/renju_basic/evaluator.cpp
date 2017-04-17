#include "../chesstype.hpp"
#include "../status.hpp"
#include "virtualboard.hpp"
#include "point.hpp"
#include "evaluator.hpp"
#include "typetree.hpp"

bool VirtualBoard::Evaluator::isInitialized = false;

void VirtualBoard::Evaluator::initialize() {
  if (isInitialized) return;
  isInitialized = true;

  TypeTree::initialize();
}

void VirtualBoard::Evaluator::evaluateType(STATUS *status, ChessType* type[2]) {
  TypeTree::classify(status, type);
}

/* score[0]:black's total score,[1]:white's */
void VirtualBoard::Evaluator::evaluateScore(ChessType* type[4][2], int *score) {
  /* count[color][length][LorD][level]
   * color: 0 for BLACK, 1 for WHITE
   * length: means the length of the type, should be 0~5.
   * LorD: 0 for LIVE, 1 for DEAD
   * e.g count[1][4][0] means the number of white dead fours */
  int count[2][6][2][4] = {{{0}}};

  /* count the types in 4 directions */
  for (int color = 0; color < 2; ++color)
    for (int d = 0; d < 4; ++d)
      switch (type[d][color]->length) {
        case -1:
          /* long connect*/
          ++(count[color][6][0][0]); break;
        case -2:
          /* double 4 in single direction*/
          (count[color][4][0][0]) += 2; break;
        default:
          ++(count[color]
             [type[d][color]->length()]
             [type[d][color]->life()]
             [type[d][color]->level()]);
      }

  /* [0] attack, [1] defense */
  const int SCORE_5[2] = {10000000, 1000000};
  const int SCORE_LIVE4[2] = {10000, 2500};
  const int SCORE_DOUBLE4[2] = {10000, 2500};
  const int SCORE_DEAD4LIVE3[2] = {7000, 2000};

  const int SCORE_DOUBLELIVE3[2] = {1500, 300};
  const int SCORE_DEAD4[2] = {250, 140};
  const int SCORE_LIVE3[2][2] = {{220, 280}, {120, 150}};
  const int SCORE_DEAD3[2] = {84, 45};
  const int SCORE_LIVE2[2][3] = {{45, 66, 88}, {20, 40, 50}};
  const int SCORE_DEAD2[2] = {25, 14};
  const int SCORE_LIVE1[2][4] = {{6, 9, 15, 20}, {2, 5, 12, 16}};
  const int SCORE_DEAD1[2] = {2, 1};

  const double SCORE_FORBIDDEN_6[2] = {-2, 0};
  const double SCORE_FORBIDDEN_ELSE[2] = {-2, 120};

  /*these const are for indexing purposes, enhancing the readibility of the code. */
  const int ATTACK = 0;
  const int DEFENSE = 1;
  const int BLACK = 0;
  const int WHITE = 1;
  const int LIVE = 0;
  const int DEAD = 1;

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

    /* self long connect */
    else if (count[selfColor][6][0] > 0)
      score[selfColor] = SCORE_FORBIDDEN_6[ATTACK];
    /* opponent long connect */
    else if (count[opponentColor][6][0] > 0)
      score[selfColor] = SCORE_FORBIDDEN_6[DEFENSE];

    /* self muliy-4 */
    else if (count[selfColor][4][LIVE] + count[selfColor][4][DEAD] >= 2)
    {
      if (selfColor == BLACK)
        score[selfColor] = SCORE_FORBIDDEN_ELSE[ATTACK];
      else
        score[selfColor] = SCORE_DOUBLE4[ATTACK];
    }
    /* opponent muliy-4 */
    else if (count[opponentColor][4][LIVE] + count[opponentColor][4][DEAD] >= 2)
    {
      if (selfColor == BLACK)
        score[selfColor] = SCORE_DOUBLE4[DEFENSE];
      else
        score[selfColor] = SCORE_FORBIDDEN_ELSE[DEFENSE];
    }

    /* is black && self multy-3 */
    else if (selfColor == BLACK && count[selfColor][3][LIVE] >= 2)
      score[selfColor] = SCORE_FORBIDDEN_ELSE[ATTACK];

    /* self l4 */
    else if (count[selfColor][4][LIVE] > 0)
      score[selfColor] = SCORE_LIVE4ATTACK];
    /* opponent l4 */
    else if (count[opponentColor][4][LIVE] > 0)
      score[selfColor] = SCORE_LIVE4[DEFENSE];

    /* self d4-l3 */
    else if (count[selfColor][4][DEAD] > 0 && count[selfColor][3][LIVE] > 0)
      score[selfColor] = SCORE_DEAD4LIVE3[ATTACK];
    /* opponent d4-l3 */
    else if (count[opponentColor][4][DEAD] > 0 && count[opponentColor][3][LIVE] > 0)
      score[selfColor] = SCORE_DEAD4LIVE3[DEFENSE];
    else
    {
      /* is white && self multi-3 */
      if (count[selfColor][3][LIVE] >= 2)
        score[selfColor] += SCORE_DOUBLELIVE3[ATTACK];

      /* opponent multi-3 */
      if (count[opponentColor][3][LIVE] >= 2)
      {
        if (selfColor == BLACK)
          score[selfColor] += SCORE_DOUBLELIVE3[DEFENSE];
        else
          score[selfColor] = SCORE_FORBIDDEN_ELSE[DEFENSE];
      }


  /* calculate score */
  for (int selfColor = BLACK, opponentColor = WHITE, i = 0; i < 2;
       selfColor = WHITE, opponentColor = BLACK, ++i) {
    /* self 5 */
    if (count[selfColor][5][0][0] > 0)
      score[selfColor] = SCORE_5[ATTACK];
    /* opponent 5 */
    else if (count[opponentColor][5][0][0] > 0)
      score[selfColor] = SCORE_5[DEFENSE];

    /* self long connect */
    else if (count[selfColor][6][0][0] > 0)
      score[selfColor] = SCORE_FORBIDDEN_6[ATTACK];
    /* opponent long connect */
    else if (count[opponentColor][6][0][0] > 0)
      score[selfColor] = SCORE_FORBIDDEN_6[DEFENSE];

    /* self muliy-4 */
    else if (count[selfColor][4][LIVE] + count[selfColor][4][DEAD] >= 2) {
      if (selfColor == BLACK)
        score[selfColor] = SCORE_FORBIDDEN_ELSE[ATTACK];
      else
        score[selfColor] = SCORE_DOUBLE4[ATTACK];
    }
    /* is black && self multy-3 */
    else if (selfColor == BLACK && count[selfColor][3][LIVE] >= 2)
      score[selfColor] = SCORE_FORBIDDEN_ELSE[ATTACK];
    /* self l4 */
    else if (count[selfColor][4][LIVE][0] > 0)
      score[selfColor] = SCORE_LIVE4[ATTACK];
    /* self d4-l3 */
    else if (count[selfColor][4][DEAD][0] > 0 &&
             (count[selfColor][3][LIVE][0] > 0 ||
             count[selfColor][3][LIVE][1] > 0))
      score[selfColor] = SCORE_DEAD4LIVE3[ATTACK];

    /* opponent muliy-4 */
    else if (count[opponentColor][4][LIVE] + count[opponentColor][4][DEAD] >= 2) {
      if (selfColor == BLACK)
        score[selfColor] = SCORE_DOUBLE4[DEFENSE];
      else
        score[selfColor] = SCORE_FORBIDDEN_ELSE[DEFENSE];
    }
    /* opponent l4 */
    else if (count[opponentColor][4][LIVE][0] > 0)
      score[selfColor] = SCORE_LIVE4[DEFENSE];
    /* opponent d4-l3 */
    else if (count[opponentColor][4][DEAD][0] > 0 &&
             (count[opponentColor][3][LIVE][0] > 0 ||
             count[opponentColor][3][LIVE][1] > 0))
      score[selfColor] = SCORE_DEAD4LIVE3[DEFENSE];
    else {
      /* self multi-3 */
      if (count[selfColor][3][LIVE][0] + count[selfColor][3][LIVE][1] >= 2) {
        score[selfColor] += SCORE_DOUBLELIVE3[ATTACK];
      } else {
        /* self live 3 */
        for (int i = 0; i < 2; ++i) {
          score[selfColor] += SCORE_LIVE3[ATTACK][i] * count[selfColor][3][LIVE][i];
        }
      }
      /* opponent multi-3 */
      if (count[opponentColor][3][LIVE][0] + count[opponentColor][3][LIVE][1] >= 2) {
        score[selfColor] += SCORE_DOUBLELIVE3[DEFENSE];
      } else {
        /* opponent live 3 */
        for (int i = 0; i < 2; ++i) {
          score[selfColor] += SCORE_LIVE3[DEFENSE][i] * count[opponentColor][3][LIVE][i];
        }
      }


      /* self dead 4 */
      if (count[selfColor][4][DEAD][0])
        score[selfColor] += SCORE_DEAD4[ATTACK];
      /* opponent dead4 */
      if (count[opponentColor][4][DEAD][0])
        score[selfColor] += SCORE_DEAD4[DEFENSE];

      /* self dead 3 */
      if (count[selfColor][3][DEAD][0])
        score[selfColor] += SCORE_DEAD3[ATTACK] * count[selfColor][3][DEAD][0];
      /* opponent dead 3 */
      if (count[opponentColor][3][DEAD][0])
        score[selfColor] += SCORE_DEAD3[DEFENSE] * count[opponentColor][3][DEAD][0];

      /* self live 2 */
      for (int i = 0; i < 3; ++i) {
        score[selfColor] += SCORE_LIVE2[ATTACK][i] * count[selfColor][2][LIVE][i];
      }
      /* opponent live 2 */
      for (int i = 0; i < 3; ++i) {
        score[selfColor] += SCORE_LIVE2[DEFENSE][i] * count[opponentColor][2][LIVE][i];
      }

      /* self dead 2 */
      if (count[selfColor][2][DEAD][0])
        score[selfColor] += SCORE_DEAD2[ATTACK] * count[selfColor][2][DEAD][0];
      /* opponent dead 2 */
      if (count[opponentColor][2][DEAD][0])
        score[selfColor] += SCORE_DEAD2[DEFENSE] * count[opponentColor][2][DEAD][0];

      /* self live 1 */
      for (int i = 0; i < 4; ++i) {
        score[selfColor] += SCORE_LIVE1[ATTACK][i] * count[selfColor][1][LIVE][i];
      }
      /* opponent live 1 */
      for (int i = 0; i < 4; ++i) {
        score[selfColor] += SCORE_LIVE1[DEFENSE][i] * count[opponentColor][1][LIVE][i];
      }

      /* self dead 1 */
      if (count[selfColor][1][DEAD][0])
        score[selfColor] += SCORE_DEAD1[ATTACK] * count[selfColor][1][DEAD][0];
      /* opponent dead 1*/
      if (count[opponentColor][1][DEAD][0])
        score[selfColor] += SCORE_DEAD1[DEFENSE] * count[opponentColor][1][DEAD][0];
    }
  }
}