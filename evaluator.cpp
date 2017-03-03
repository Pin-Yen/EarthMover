#include "chessboard.hpp"


int* evaluate(ChessType* type[4][2], STATUS *status, int dir, bool checkForbidden)
{

  type[dir] = TypeTree::classify(status, checkForbidden);

  /* score[0]:black's total score,[1]:white's */
  int score[2] = {0};

  /* count[color][length][LorD]
   * color: 0 for BLACK, 1 for WHITE
   * length: means the length of the type, should be 1~6.
   * LorD: 0 for LIVE, 1 for DEAD 
   * e.g count[1][4][0] means the number of white dead fours */
  int count[2][7][2] = {{{0}}};


  /* count the types in 4 directions */
  for(int color = 0; color < 2; ++color)
  	for(int d = 0; d < 4; ++d)
  		++(count[ color ][ type[d][color]->length ][ type[d][color]->life ]);

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
  const int SCORE LIVE1[2] = {25, 10};
  const int SCORE_DEAD1[2] = {4, 1};

  #############################################################
  SHOULD WE DISCOURAGE WIHTE TO PLAY BLACK`s FORBIDDEN POINTS ?
  #############################################################
  const int SCORE_FORBIDDEN[2] = {-10000,????};
  
  ############################################
  ###### ARE THESE #DEFINE OK? ###############
  ############################################
  #define ATTACK 0
  #define DEFENSE 1
  #define BLACK 0
  #define WHITE 1
  #define LIVE 0
  #define DEAD 1

 

  /*** calculate score */
  for(int selfColor = BLACK, opponentColor = WHITE, count = 0; count < 2; selfColor = WHITE, opponentColor = BLACK, ++count){
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
  	else if(count[opponentColor][4][LIVE] + count[opponent][4][DEAD] >= 2)
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
  	/* six, multi-four or multi-live-three */
  	if((count[BLACK][6][DEAD] != 0 || count[BLACK][6][LIVE] != 0)
  		||(count[BLACK][4][DEAD] + count[BLACK][4][LIVE] >= 2)
  		||(count[BLACK][3][LIVE] >= 2 )){

  		#####################################
  		CONFUSING HERE, `SCORE_FORBIDDEN[ATTACK]` ISN`T A ATTACKIVE MOVE. 
  		####################################
  		score[BLACK] = SCORE_FORBIDDEN[ATTACK]; 
  		score[WHITE] = SCORE_FORBIDDEN[DEFENSE];  		
  	}
  }
  return score;
}
