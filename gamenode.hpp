#ifndef GAMENODE_HPP
#define GAMENODE_HPP
class GameNode
{
public:

	GameNode(GameNode *parentNode , int row, int col){
		/* initialize all childNodes to NULL, all childProp to 0 */
		for(int i=0; i<CHESSBOARD_DIMEN; ++i){
			for(int k=0; k<CHESSBOARD_DIMEN; ++k){
				childNode[i][k] = NULL;
				childProp[i][k] = 0;
			}
		}
		/* initialize counters to 0 */
		totalPlayout = 0;
		winPlayout = 0;
		losePlayout = 0;

		/* initialize board
		 * if parentNode exists, create a board from parents board,
		 * else create a blank one. */
		if(parentNode != NULL){
			board = new VirtualBoard(parentNode->board);
			board.play(row, col);
		}
		else{
			board = new VirtualBoard();
		}

		/* initiaize parent node */
		parent = parentNode;
	}

	~GameNode(){
		for(int i=0; i<CHESSBOARD_DIMEN; ++i){
			for(int k=0; k<CHESSBOARD_DIMEN; ++k){
				if(childNode[i][k] != NULL){
					delete childNode[i][k];
				}
			}
		}
	}

	/* backprop, result: 0=lose, 1=win */
	void updateAndBackprop(int result){
		++totalPlayout;
		updateProb(row, col, calcProb());
		if(result == 0)
			++losePlayout;
		else if(result == 1)
			++winPlayout;
		if(parent != NULL)
			parent->updateAndBackprop(result);
	}

	/* get childNode, if it's NULL, construct one */
	Node* getChildNode(int r, int c){
		if(childNode[r][c] == NULL)
			childNode[r][c] = new GameNode(this, r, c);
		return childNode[r][c];
	}

	/* pick a random point according to the probability */
	Node* pickRandomPoint(){
		//TODO:
	}


private:
	  Node *childNode[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
    int childProb[CHESSBOARD_DIMEN][CHESSBOARD_DIMEN];
    int totalPlayout, winPlayout, losePlayout; 
    int row, col;
    VirtualBoard* board;
  	Node *parent;

  	/* calculates a probability for this point */
  	calcProb(){
  		//TODO: write a function
  	}
};
#endif