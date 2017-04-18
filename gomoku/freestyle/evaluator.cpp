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
  //len, LorD, lev, col
  const int SCORE[6][2][4][2] = {{{{0, 0}, {0, 0}, {0, 0}, {0, 0}},             /* 0 */
                                  {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},            /* X */
                                 {{{2, 1}, {0, 0}, {0, 0}, {0, 0}},             /* D1 */
                                  {{6, 2}, {9, 5}, {15, 12}, {20, 16}}},        /* L1 */
                                 {{{25, 14}, {0, 0}, {0, 0}, {0, 0}},           /* D2 */
                                  {{45, 20}, {66, 40}, {88, 50}, {0, 0}}},      /* L2 */
                                 {{{84, 45}, {0, 0}, {0, 0}, {0, 0}},           /* D3 */
                                  {{220, 120}, {280, 150}, {0, 0}, {0, 0}}},    /* L3 */
                                 {{{250, 150}, {0, 0}, {0, 0}, {0, 0}},         /* D4 */
                                  {{10000, 1000}, {0, 0}, {0, 0}, {0, 0}}},     /* L4 */
                                 {{{10000000, 500000}, {0, 0}, {0, 0}, {0, 0}}, /* 5 */
                                  {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}};           /* X */

  const int SCORE_DOUBLE4[2] = {10000, 1000};
  const int SCORE_DEAD4LIVE3[2] = {8000, 800};
  const int SCORE_DOUBLELIVE3[2] = {800, 200};

  /* these const are for indexing purposes, enhancing the readibility of the code. */
  const int ATTACK = 0;
  const int DEFENSE = 1;
  const int BLACK = 0;
  const int WHITE = 1;
  const int LIVE = 1;
  const int DEAD = 0;

  score[BLACK] = 0; score[WHITE] = 0;

  /* count[color][length][LorD]
   * color: 0 for BLACK, 1 for WHITE
   * length: means the length of the type, should be 0~5.
   * LorD: 0 for LIVE, 1 for DEAD
   * e.g count[1][4][0] means the number of white dead fours */
  int count[2][6][2] = {0};

  for (int selfColor = BLACK, opponentColor = WHITE, i = 0; i < 2;
       selfColor = WHITE, opponentColor = BLACK, ++i) {
    for (int d = 0; d < 4; ++d) {
      /* count the types in 4 directions */
      ++(count[selfColor]
              [type[d][selfColor]->length()]
              [type[d][selfColor]->life()]);

      /* add score */
      score[selfColor] += SCORE[type[d][selfColor]->length()]
                               [type[d][selfColor]->life()]
                               [type[d][selfColor]->level()]
                               [0];
      score[opponentColor] += SCORE[type[d][selfColor]->length()]
                                   [type[d][selfColor]->life()]
                                   [type[d][selfColor]->level()]
                                   [1];
    }
  }

  /* [0] attack, [1] defense */

  /* calculate score */
  for (int selfColor = BLACK, opponentColor = WHITE, i = 0; i < 2;
       selfColor = WHITE, opponentColor = BLACK, ++i) {
    if (count[selfColor][5][0] > 0 ||
        count[opponentColor][5][0] > 0 ||
        count[selfColor][4][LIVE] > 0) {
      continue;
    } else if (count[selfColor][4][DEAD] >= 2) {
      /* self muliy-4 */
      score[selfColor] += SCORE_DOUBLE4[ATTACK];
    } else if (count[selfColor][4][DEAD] > 0 &&
               count[selfColor][3][LIVE] > 0) {
      /* self d4-l3 */
      score[selfColor] += SCORE_DEAD4LIVE3[ATTACK];
    } else if (count[opponentColor][4][DEAD] >= 2) {
      /* opponent muliy-4 */
      score[selfColor] += SCORE_DOUBLE4[DEFENSE];
    } else if (count[opponentColor][4][DEAD] > 0 &&
               count[opponentColor][3][LIVE] > 0) {
      /* opponent d4-l3 */
      score[selfColor] += SCORE_DEAD4LIVE3[DEFENSE];
    } else if (count[selfColor][3][LIVE] >= 2) {
      /* self multi-3 */
      score[selfColor] += SCORE_DOUBLELIVE3[ATTACK];
    } else if (count[opponentColor][3][LIVE] >= 2) {
      /* opponent multi-3 */
      score[selfColor] += SCORE_DOUBLELIVE3[DEFENSE];
    }
  }
}

void VirtualBoard::Evaluator::evaluateRelativeScore(VirtualBoard::Point* point[15][15],
                                                    int playNo) {
  const int USING_OPEINIG = 0;

  if (playNo <= USING_OPEINIG) {
    /* using opening tree */
    switch (playNo) {
      case 0: /* first move(set center point to 1) */
        for (int r = 0; r < 15; ++r)
          for (int c = 0; c < 15; ++c)
            point[r][c]->setRelScore(-1);

        point[7][7]->setRelScore(1);
        break;
    }

  } else {
    /* using absloute score */

    bool whoTurn = playNo & 1;

    /* get highest score */
    int highestScore = -1;
    for (int r = 0; r < 15; ++r)
      for (int c = 0; c < 15; ++c)
        if (point[r][c]->absScore(whoTurn) > highestScore)
          highestScore = point[r][c]->absScore(whoTurn);

    for (int r = 0; r < 15; ++r)
      for (int c = 0; c < 15; ++c) {
        int score = point[r][c]->absScore(whoTurn);
        if (score * 10 < highestScore)
          point[r][c]->setRelScore(-1);
        else
          point[r][c]->setRelScore(score);
      }
  }
}