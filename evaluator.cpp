#include "chessboard.hpp"


int* evaluate(ChessType* type[4][2], STATUS *status, int dir, bool checkForbidden)
{

  type[dir] = TypeTree::classify(status, checkForbidden);

  /* score[0]:black's [1]:white's */
  int score[2] = {0};

  //TODO: score this point according the type[4][2]

  /* count[color][length][LorD]
   * color: 0 for BLACK, 1 for WHITE
   * length: means the length of the type, should be 1~6.
   * LorD: 0 for LIVE, 1 for DEAD 
   * e.g count[1][4][0] means the number of white dead fours */
  int count[2][7][2] = {{{0}}};


  /* count the types in 4 directions */
  for(int color = 0; color < 2; ++color)
  	for(int d = 0; d < 4; ++d)
  		++(count[color][type[d][color]->length][type[d][color]->life]);

  /* [0] attack, [1] defense*/
  const int SCORE_6[2] = {}
  const int SCORE_5[2] = {1000, 900};
  const int SCORE_DOUBLE4[2] = {760, 700};
  const int SCORE_LIVE4[2] = {760, 700};
  const int SCORE_DEAD4LIVE3 = {720, 660};

  const int SCORE_DOUBLELIVE3 = {400, 320};
  const int SCORE_DEAD4[2] = {220, 160};
  const int SCORE_LIVE3[2] = {200, 120};
  const int SCORE_DEAD3[2] = {80, 50};
  const int SCORE_LIVE2[2] = {80, 50};
  const int SCORE_DEAD2[2] = {25, 10};
  const int SCORE LIVE1[2] = {25, 10};
  const int SCORE_DEAD1[2] = {4, 1};

  const int SCORE_FORBIDDEN[2] = {-10000,??};
  #########HELP ME#################


  for(int color=0; color<2; ++color){
  	for(int SelfOrOpponent=0; SelfOrOpponent<2; ++SelfOrOpponent){
  		/* check forbidden points */
  		if(checkForbidden && ((color==0 && SelfOrOpponent == 0))||(color==1 && SelfOrOpponent==1)){
  			/* six */
  			if(count[0][6][0]!= 0 || count[0][6][1] != 0){
  				score[color] += SCORE_FORBIDDEN[SelfOrOpponent];
  			}
  			/* double 4 or double live 3 */
  			if((count[0][4][0]+count[0][4][1] >= 2) || count[0][3][1] >= 2){
  				score[color] += SCORE_FORBIDDEN[SelfOrOpponent]
  			}
  		}

  		/* check critical points*/
  		bool critical = false;
  		/* self 5 */
  		if(count[color][5][0] + count[color][5][1] != 0 && SelfOrOpponent == 0){
  			 score[color] += SCORE_5[0];
  		}
  		/* opponent 5 */
  		if(count[])
  	}
  }


  return score;
}
