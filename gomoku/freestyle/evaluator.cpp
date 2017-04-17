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
  int count[2][6][2][4] = {{{{0}}}};

  /* count the types in 4 directions */


  //len, LorD, lev, col
  const int SCORE[6][2][4][2] = {{{{0, 0}, {0, 0}, {0, 0}, {0, 0}},
                                  {{0, 0}, {0, 0}, {0, 0}, {0, 0}}},
                                 {{{2, 1}, {0, 0}, {0, 0}, {0, 0}},
                                  {{6, 2}, {9, 5}, {15, 12}, {20, 16}}},
                                 {{{25, 14}, {0, 0}, {0, 0}, {0, 0}},
                                  {{45, 20}, {66, 40}, {88, 50}, {0, 0}}},
                                 {{{84, 45}, {0, 0}, {0, 0}, {0, 0}},
                                  {{220, 120}, {280, 150}, {0, 0}, {0, 0}}},
                                 {{{250, 140}, {0, 0}, {0, 0}, {0, 0}},
                                  {{25000, 2500}, {0, 0}, {0, 0}, {0, 0}}},
                                 {{{10000000, 500000}, {0, 0}, {0, 0}, {0, 0}},
                                  {{0, 0}, {0, 0}, {0, 0}, {0, 0}}}};

  score[0] = 0; score[1] = 0;

  for (int color = 0, opponentColor = 1; color < 2; ++color, opponentColor = 0)
    for (int d = 0; d < 4; ++d) {
      ++(count[color]
              [type[d][color]->length()]
              [type[d][color]->life()]
              [type[d][color]->level()]);

      score[color] += SCORE[type[d][color]->length()]
                           [type[d][color]->life()]
                           [type[d][color]->level()]
                           [0];
      score[opponentColor] += SCORE[type[d][color]->length()]
                                   [type[d][color]->life()]
                                   [type[d][color]->level()]
                                   [1];
    }

  /* [0] attack, [1] defense */
                                  /*
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
  const int SCORE_DEAD1[2] = {2, 1};*/

  const int SCORE_DOUBLE4[2] = {25000, 2500};
  const int SCORE_DEAD4LIVE3[2] = {20000, 2000};
  const int SCORE_DOUBLELIVE3[2] = {1000, 200};

  /* these const are for indexing purposes, enhancing the readibility of the code. */
  const int ATTACK = 0;
  const int DEFENSE = 1;
  const int BLACK = 0;
  const int WHITE = 1;
  const int LIVE = 1;
  const int DEAD = 0;

  /* calculate score */
  for (int selfColor = BLACK, opponentColor = WHITE, i = 0; i < 2;
       selfColor = WHITE, opponentColor = BLACK, ++i) {
    if (count[selfColor][4][DEAD][0] >= 2) {
      /* self muliy-4 */
      score[selfColor] += SCORE_DOUBLE4[ATTACK];
    } else if (count[selfColor][4][DEAD][0] > 0 &&
             (count[selfColor][3][LIVE][0] > 0 ||
             count[selfColor][3][LIVE][1] > 0)) {
      /* self d4-l3 */
      score[selfColor] += SCORE_DEAD4LIVE3[ATTACK];
    } else if (count[opponentColor][4][DEAD][0] >= 2) {
      /* opponent muliy-4 */
      score[selfColor] += SCORE_DOUBLE4[DEFENSE];
    } else if (count[opponentColor][4][DEAD][0] > 0 &&
             (count[opponentColor][3][LIVE][0] > 0 ||
             count[opponentColor][3][LIVE][1] > 0)) {
      /* opponent d4-l3 */
      score[selfColor] += SCORE_DEAD4LIVE3[DEFENSE];
    } else if (count[selfColor][3][LIVE][0] + count[selfColor][3][LIVE][1] >= 2) {
      /* self multi-3 */
      score[selfColor] += SCORE_DOUBLELIVE3[ATTACK];
    } else if (count[opponentColor][3][LIVE][0] + count[opponentColor][3][LIVE][1] >= 2) {
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
    //switch (playNo) {
      //case 0: /* first move(set center point to 1) */
        for (int r = 0; r < 15; ++r)
          for (int c = 0; c < 15; ++c)
            point[r][c]->setRelScore(-1);

        point[7][7]->setRelScore(1);
        //break;
    //}

  } else {
    /* using absloute score */

    bool whoTurn = playNo & 1;

    /* get highest score */
    int highestScore = -1;
    for (int r = 0; r < 15; ++r)
      for (int c = 0; c < 15; ++c)
        if (point[r][c]->absScore(whoTurn) > highestScore)
          highestScore = point[r][c]->absScore(whoTurn);

    if (highestScore >= 10000000) {
      /* if exist five */
      for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 15; ++c) {
          if (point[r][c]->absScore(whoTurn) < highestScore)
            point[r][c]->setRelScore(-1);
          else
            point[r][c]->setRelScore(highestScore);
        }
      }
    } else if (highestScore >= 500000) {
      /* if exist opponent five */
      for (int r = 0; r < 15; ++r) {
        for (int c = 0; c < 15; ++c) {
          if (point[r][c]->absScore(whoTurn) < highestScore)
            point[r][c]->setRelScore(-1);
          else
            point[r][c]->setRelScore(highestScore);
        }
      }
    } else {
      for (int r = 0; r < 15; ++r)
        for (int c = 0; c < 15; ++c)
          point[r][c]->setRelScore(point[r][c]->absScore(whoTurn));
    }
  }
}