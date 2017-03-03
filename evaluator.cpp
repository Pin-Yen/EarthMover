#include "displayboard.hpp"
#include "evaluator.hpp"
/* score[0]:black's total score,[1]:white's */
void evaluate(ChessType* type[4][2], STATUS *status, int dir, bool checkForbidden,int *score)
{

  TypeTree::classify(status, checkForbidden, (type[dir]));

  /* count[color][length][LorD]
   * color: 0 for BLACK, 1 for WHITE
   * length: means the length of the type, should be 1~6.
   * LorD: 0 for LIVE, 1 for DEAD 
   * e.g count[1][4][0] means the number of white dead fours */
  int count[2][7][2] = {{{0}}};


  /* count the types in 4 directions */
  for(int color = 0; color < 2; ++color)
  	for(int d = 0; d < 4; ++d){
  		if(type[d][color]->length == -1) /* double 4 in single direction*/
  			(count[ color ][4][type[d][color]->life]) += 2;
  		else
  			++(count[ color ][ type[d][color]->length ][ type[d][color]->life ]);
  	}

  /* [0] attack, [1] defense*/
  const int SCORE_5[2] = {1000, 900};
  const int SCORE_DOUBLE4[2] = {760, 700};
  const int SCORE_LIVE4[2] = {760, 700};
  const int SCORE_DEAD4LIVE3[2] = {720, 660};

  const int SCORE_DOUBLELIVE3[2] = {400, 320};
  const int SCORE_DEAD4[2] = {220, 160};
  const int SCORE_LIVE3[2] = {200, 120};
  const int SCORE_DEAD3[2] = {80, 50};
  const int SCORE_LIVE2[2] = {80, 50};
  const int SCORE_DEAD2[2] = {25, 10};
  const int SCORE_LIVE1[2] = {25, 10};
  const int SCORE_DEAD1[2] = {4, 1};

  const int SCORE_FORBIDDEN_6[2] = {-10000, 0};
  const int SCORE_FORBIDDEN_ELSE[2] = {-10000, 160};

	/*these const are for indexing purposes, enhancing the readibility of the code. */
  int ATTACK = 0;
  int DEFENSE = 1;
  int BLACK = 0;
  int WHITE = 1;
  int LIVE = 0;
  int DEAD = 1;
 

  /*** calculate score */
  for(int selfColor = BLACK, opponentColor = WHITE, i = 0; i < 2; selfColor = WHITE, opponentColor = BLACK, ++i){
  	/* self 5 */
  	if(count[selfColor][5][DEAD] != 0 || count[selfColor][5][LIVE] != 0)
  		score[selfColor] += SCORE_5[ATTACK];
  	/* opponent 5 */
  	else if(count[opponentColor][5][DEAD] != 0 || count[opponentColor][5][LIVE] != 0)
  		score[selfColor] += SCORE_5[DEFENSE];
  	/* self multi-4 */
  	else if(count[selfColor][4][LIVE] + count[selfColor][4][DEAD] >= 2)
  		score[selfColor] += SCORE_DOUBLE4[ATTACK];
  	/* opponent multi-4*/
  	else if(count[opponentColor][4][LIVE] + count[opponentColor][4][DEAD] >= 2)
  		score[selfColor] += SCORE_DOUBLE4[DEFENSE];
  	/* self d4-l3 */
  	else if (count[selfColor][4][LIVE] > 0)
  		score[selfColor] += SCORE_DEAD4LIVE3[ATTACK];
  	/* opponent d4-l3*/
  	else if(count[opponentColor][4][LIVE] > 0)
  		score[selfColor] += SCORE_DEAD4LIVE3[DEFENSE];
  	else{
  		/* self live 3 */
  		if(count[selfColor][3][LIVE])
  			score[selfColor] += SCORE_LIVE3[ATTACK];
  		/* opponent live 3 */
  		if(count[opponentColor][3][LIVE])
  			score[selfColor] += SCORE_LIVE3[DEFENSE];
  		/* self dead 3 */
  		if(count[selfColor][3][DEAD])
  			score[selfColor] += SCORE_DEAD3[ATTACK];  			
  		/* opponent dead 3 */
  		if(count[opponentColor][3][DEAD])
  			score[selfColor] += SCORE_DEAD3[DEFENSE];

  		/* self live 2 */
  		if(count[selfColor][2][LIVE])
  			score[selfColor] += SCORE_LIVE2[ATTACK];
  		/* opponent live 2 */
  		if(count[opponentColor][2][LIVE])
  			score[selfColor] += SCORE_LIVE2[DEFENSE];
  		/* self dead 2 */
  		if(count[selfColor][2][DEAD])
  			score[selfColor] += SCORE_DEAD2[ATTACK];  			
  		/* opponent dead 2 */
  		if(count[opponentColor][2][DEAD])
  			score[selfColor] += SCORE_DEAD2[DEFENSE];

  		/* self live 1 */
  		if(count[selfColor][1][LIVE])
  			score[selfColor] += SCORE_LIVE1[ATTACK];
  		/* opponent live 1 */
  		if(count[opponentColor][1][LIVE])
  			score[selfColor] += SCORE_LIVE1[DEFENSE];
  		/* self dead 1 */
  		if(count[selfColor][1][DEAD])
  			score[selfColor] += SCORE_DEAD1[ATTACK];  			
  		/* opponent dead 1*/
  		if(count[opponentColor][1][DEAD])
  			score[selfColor] += SCORE_DEAD1[DEFENSE];
  	}
  }

 	/* check forbidden points */
  if(checkForbidden){
  	/* six */
  	if(count[BLACK][6][DEAD] != 0 || count[BLACK][6][LIVE] != 0){
  		score[BLACK] = SCORE_FORBIDDEN_6[ATTACK];
  		score[WHITE] = SCORE_FORBIDDEN_6[DEFENSE];
  	}
  	/* multi-four or multi-live-three */
  	else if((count[BLACK][4][DEAD] + count[BLACK][4][LIVE] >= 2)
  		||(count[BLACK][3][LIVE] >= 2 )){
  		score[BLACK] = SCORE_FORBIDDEN_ELSE[ATTACK]; 
  		score[WHITE] = SCORE_FORBIDDEN_ELSE[DEFENSE];  		
  	}
  }
}
