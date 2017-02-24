#include "typetree.hpp"
#include <iostream>

TypeTree::TypeTree(){
	STATUS status[length];
	dfs(&root, status, length/2, -1);
}

/* Depth First Search
 * parameters of the initial call should be:
 * currentLocation: length/2, move = -1 */
void TypeTree::dfs(Node *root, STATUS *status, int currentLocation, int move){
	if(status[currentLocation] == DIFFERENT || currentLocation < 0 || currentLocation > length){
		if(move == 1){
			// reached leaf.
			root->type = typeAnalyze(length, status, checkLongConnect);
			print(length,status,root->type);
			return;
		}
		else
			move += 2;
	}

	int nextLocation = currentLocation += move;

	/* let next location be the same color */
	root->childNode[0] = (Node*)malloc(sizeof(Node));
	status[currentLocation] = SAME;
	dfs(root->childNode[0], status, nextLocation, move);

	/* let next location be different color */
	root->childNode[1] = (Node*)malloc(sizeof(Node));
	status[currentLocation] = DIFFERENT;
	dfs(root->childNode[1], status, nextLocation, move);		

	/* let next location be empty */
	root->childNode[2] = (Node*)malloc(sizeof(Node));
	status[currentLocation] = EMPTY;
	dfs(root->childNode[2], status, nextLocation, move);

	/* restore current location to NO_MATTER
	 * note: without this line, the classification process should  still work fine ,
	 * but will result in printing out garbage values on NO_MATTER points */
	status[currentLocation] = NO_MATTER;

}

ChessType * TypeTree::typeAnalyze(int length, STATUS *status, bool checkLongConnect)
{
  int connect = 1;

  /* check the length of the connection around the analize point*/
  /* under the following, we call this chess group "center group" (CG)*/
  /* for example : --X O*OOX-- ; OOOO* O X */
  /*                   ^^^^      ^^^^^     */
  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, n = start; i < 4; ++i, n += move)
    {
      if (status[n] != SAME)
        break;

      ++connect;
    }

  if (connect > 5)
    /* CG's length > 5, if check long connect, return 6, else return 5*/
    return (checkLongConnect ? (new ChessType(6, 0)) : (new ChessType(5, 0)));
  else if (connect == 5)
    /* CG's length == 5, return 5*/
    return new ChessType(5, 0);
  else
  {
    /* CG's length < 5*/

    /* play at the analize point*/
    status[length / 2] = SAME;

    /* try to find the left and right bound of CG*/
    /* if it's empty, see this point as new analize point*/
    /* make a new status array and use recursion analize the status*/
    ChessType *lStyle, *rStyle;

    for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, n = start; count < 4; ++count, n += move)
      {
        /* if the bound is an empty point*/
        if (status[n] == EMPTY)
        {
          /* make a new status array*/
          STATUS newStatus[length];

          /* translate from origin status*/
          for (int i = 0; i < length; ++i)
          {
            if (i == length / 2)
              /* if i = center of length, it's analize point*/
              newStatus[i] = ANALYZE_POINT;
            else if ((i - (length / 2 - n)) < 0 || (i - (length / 2 - n)) >= length)
              /* length / 2 - n means translation magnitude*/
              /* if out of bound, see it as empty point*/
              newStatus[i] = EMPTY;
            else
              newStatus[i] = status[i - (length / 2 - n)];
          }

          /* recursion analize*/
          if (move == -1)
            lStyle = typeAnalyze(length, newStatus, checkLongConnect);
          else
            rStyle = typeAnalyze(length, newStatus, checkLongConnect);
          
          break;
        }
        /* if the board of CG is opponent chess, it means blocked*/
        else if (status[n] == DIFFERENT)
        {
          if (move == -1)
            lStyle = new ChessType(0, 0);
          else
            rStyle = new ChessType(0, 0);
          break;
        }
        
      }

    /* restore the analize point */
    status[length / 2] = ANALYZE_POINT;

    /* keep lStyle > rStyle*/
    if (lStyle->length < rStyle->length || 
      (lStyle->length == rStyle->length && (lStyle->life < rStyle->life)))
    {
      ChessType* temp = lStyle;
      lStyle = rStyle;
      rStyle = temp;
    }

    if (lStyle->length == 5 && rStyle->length == 5)
      /* if left and right will both produce 5 after play at analize point,*/
      /* it is a life four style*/
      return new ChessType(4, 1);
    else if (lStyle->length == 5)
      /* if there is only one side produce 5 after play at analize point,*/
      /* it is a dead four style*/
      return new ChessType(4, 0);
    else if (lStyle->length == 6 || lStyle-> length == 0)
      /* if the longer size produce 6 or 0 after play at analize point,*/
      /* it is a useless point*/
      return new ChessType(0, 0);
    else
      /* if left length < 4, return left length - 1*/
      /* (current recursion's result = lower recursion's result - 1) */
      return new ChessType(lStyle->length - 1, lStyle->life);
  }
}

void TypeTree::print(int length, STATUS *status, ChessType *type)
{
  /* print status array*/
  for (int i = 0; i < length; ++i)
    std::cout << (char)status[i];

  std::cout << " type: ";

  if (type->length > 0 && type->length < 5)
    /* print life or dead only if length == 1, 2, 3, 4*/
    std::cout << (type->life == 1 ? "life" : "dead") << " ";

  std::cout << type->length << "\n";
}


int main(){
	TypeTree *tree = new TypeTree;
	return 0;
}