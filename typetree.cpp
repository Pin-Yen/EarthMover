#include "typetree.hpp"
#include <iostream>

//TypeTree::TypeTree()

/* initialize root*/
TypeTree::Node* TypeTree::commonTree_root = (Node*)malloc(sizeof(Node));
TypeTree::Node* TypeTree::forbiddenTree_root = (Node*)malloc(sizeof(Node));

void TypeTree::initialize()
{

  /* build common tree and forbidden tree*/
  for (int length = 9; length <= 11; length += 2)
  {
    /* initialize status*/
    STATUS status[length];
    for (int i = 0; i < length; i++)
    {
      if (i == length / 2)
        status[i] = ANALYZE_POINT;
      else
        status[i] = NO_MATTER;
    }

    /* build tree*/
    if (length == 9)
    {
      dfs(commonTree_root, status, length / 2, -1, 0, false);
      cutSameResultChild(commonTree_root);
    }
    else
    {
      dfs(forbiddenTree_root, status, length / 2, -1, 0, true);
      cutSameResultChild(forbiddenTree_root);
    }
  }
}

/* Depth First Search
 * parameters of the initial call should be:
 * location: length / 2, move = -1, connect = 0 */

/* connect is used to prevent already exist five while length == 11 
 * for example : OOOOO*OOX-- ; --X  *OOOOO 
 *               ^^^^^               ^^^^^ */
void TypeTree::dfs(Node *root, STATUS *status, int location, 
  int move, int connect, bool checkForbidden)
{
  int length = (checkForbidden ? 11 : 9);

  if (status[location] == DIFFERENT || location <= 0 || location >= length - 1)
  {
    if (move == 1)
    {
      // reached leaf.
      root->type = typeAnalyze(status, checkForbidden);
      print(length, status, root->type);
      return;
    }
    else
    {
      /* jump to middle of the status*/
      move += 2;
      location = length / 2;

      /* reset connect*/
      if (checkForbidden) connect = 0;
    }
  }
  else if (status[location] == SAME)
    /* if check forbidden and color == same, increase connect*/
    if (checkForbidden)
      ++connect;

  /* move location*/
  location += move;

  /* if connect == 4, stop playing same color at this point to prevent appearing five*/
  if (connect < 4)
  {
    /* let next location be the same color */
    root->childNode[0] = (Node*)malloc(sizeof(Node));
    status[location] = SAME;
    dfs(root->childNode[0], status, location, move, connect, checkForbidden);
  }

  /* let next location be different color */
  root->childNode[1] = (Node*)malloc(sizeof(Node));
  status[location] = DIFFERENT;
  dfs(root->childNode[1], status, location, move, connect, checkForbidden);    

  /* let next location be empty */
  root->childNode[2] = (Node*)malloc(sizeof(Node));
  status[location] = EMPTY;
  dfs(root->childNode[2], status, location, move, connect, checkForbidden);

  /* restore current location to NO_MATTER
   * note: without this line, the classification process should  still work fine ,
   * but will result in printing out garbage values on NO_MATTER points */
  status[location] = NO_MATTER;
}

ChessType* TypeTree::cutSameResultChild(Node *root)
{

  ChessType* currentType;

  if (root->type != NULL) 
  {
    currentType = root->type;
  }

  for (int i = 0; i < 3; ++i)
    if (root->childNode[i] != NULL)
    {
      /* if this child node is not NULL, recursion and get return*/
      ChessType* returnType = cutSameResultChild(root->childNode[i]);

      /* if children cannot be cut, then this node also cannot be cut*/
      if (returnType == NULL) return NULL;

      if (currentType == NULL)
      {
        currentType = returnType;
      }
      else
      {
        /* if different child has different result, cannot cut this node*/
        if (currentType->length != returnType->length ||
          currentType->life != returnType->life) return NULL;
      }
    }

  /* cut this node, free all children and set this node's type*/
  root->type = currentType;

  for (int i = 0; i < 3; ++i)
    if (root->childNode[i] != NULL)
      free(root->childNode[i]);

  return currentType;
}

ChessType* TypeTree::classify(STATUS *status, bool checkForbidden)
{
  int length = checkForbidden ? 11 : 9;

  Node* node;

  /* switch root*/
  if (checkForbidden)
    node = forbiddenTree_root;
  else
    node = commonTree_root;

  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, checkPoint = start; i < length / 2; ++i, checkPoint += move)
    {
      /* according to the status to enter the child node*/
      switch(status[checkPoint])
      {
        case SAME:
          node = node->childNode[0]; break;
        case DIFFERENT:
          node = node->childNode[1]; break;
        default:
          node = node->childNode[2];
      }
      
      /* if reach leaf, return type*/
      if (node->type != NULL)
        return node->type;

      /* if reach different color, change direction*/
      if (status[checkPoint] == DIFFERENT)
        break;
    }
} 

ChessType* TypeTree::typeAnalyze(STATUS *status, bool checkForbidden)
{
  int length = checkForbidden ? 11 : 9;

  int connect = 1;

  /* check the length of the connection around the analize point
   * under the following, we call this chess group "center group" (CG)
   * for example : --X O*OOX-- ; OOOO* O X 
   *                   ^^^^      ^^^^^     */
  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, checkPoint = start; i < 4; ++i, checkPoint += move)
    {
      if (status[checkPoint] != SAME) break;

      ++connect;
    }

  if (connect > 5)
    /* CG's length > 5, if check forbidden, return -1, else return 5*/
    return (checkForbidden ? (new ChessType(-1, 0)) : (new ChessType(5, 0)));
  else if (connect == 5)
    /* CG's length == 5, return 5*/
    return new ChessType(5, 0);
  else
  {
    /* CG's length < 5*/

    /* play at the analize point*/
    status[length / 2] = SAME;

    /* try to find the left and right bound of CG
     * if it's empty, see this point as new analize point
     * make a new status array and use recursion analize the status*/
    ChessType *lType, *rType;

    for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, checkPoint = start; count < 4; ++count, checkPoint += move)
        /* if reach CG's bound*/
        if (status[checkPoint] != SAME)
        {
          ChessType* type;

          /* if the bound is an empty point*/
          if (status[checkPoint] == EMPTY)
          {
            /* make a new status array*/
            STATUS newStatus[length];

            /* transform from origin status*/
            for (int i = 0; i < length; ++i)
            {
              if (i == length / 2)
                /* if i = center of length, it's analize point*/
                newStatus[i] = ANALYZE_POINT;
              else
              { 
                /* length / 2 - n means transformation magnitude*/
                int transformation_index = i - (length / 2 - checkPoint);

                if (transformation_index < 0 || transformation_index >= length)
                  /* if out of bound, see it as empty point*/
                  newStatus[i] = EMPTY;
                else
                  newStatus[i] = status[transformation_index];
              }
            }

            /* recursion analize*/
            type = typeAnalyze(newStatus, checkForbidden);
          }
          /* if the board of CG is opponent chess, it means blocked*/
          else if (status[checkPoint] == DIFFERENT)
            type = new ChessType(0, 0);
          
          /* set analize result l/rType*/
          if (move == -1)
            lType = type;
          else
            rType = type;

          break;
        }


    /* restore the analize point */
    status[length / 2] = ANALYZE_POINT;

    /* keep lType > rType*/
    if (lType->length < rType->length || 
      (lType->length == rType->length && (lType->life < rType->life)))
    {
      ChessType* temp = lType;
      lType = rType;
      rType = temp;
    }

    if (lType->length == 5 && rType->length == 5)
      /* if left and right will both produce 5 after play at analize point,*/
    {
      if (checkForbidden && connect < 4)
        return new ChessType(-1, 0);
      else
        return new ChessType(4, 1);
    }
      
    else if (lType->length == 5)
      /* if there is only one side produce 5 after play at analize point,
       * it is a dead four style*/
      return new ChessType(4, 0);
    else if (lType->length == 0 || lType-> length == -1)
      /* if the longer size produce 6 or 0 after play at analize point,
       * it is a useless point*/
      return new ChessType(0, 0);
    else
      /* if left length < 4, return left length - 1
       * (current recursion's result = lower recursion's result - 1) */
      return new ChessType(lType->length - 1, lType->life);
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

  if (type->length == -1)
    std::cout << "forbidden point";
  else
    std::cout << type->length;

  std::cout << std::endl;
}

int main()
{
  //TypeTree *tree = new TypeTree();
  TypeTree::initialize();
}