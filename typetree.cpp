#include "typetree.hpp"
#include <iostream>
#include <iomanip>

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
    Node* root = (length == 9 ? commonTree_root : forbiddenTree_root);

    /* ####################################### debug */
    count = 0;


    dfs(root, status, length / 2, -1, 0, 0, false, false, length != 9);
    cutSameResultChild(root);

    /* ####################################### debug */
    count = 0;
    searchAll(root, status, length, length / 2, -1);

  }
}

/* ####################################### debug */
int TypeTree::count;

/* Depth First Search
 * parameters of the initial call should be:
 * location: length / 2, move = -1, connect = 0 */

/* connect is used to prevent already exist five while length == 11 
 * for example : OOOOO*OOX-- ; --X  *OOOOO 
 *               ^^^^^               ^^^^^ */
void TypeTree::dfs(Node *root, STATUS *status, int location, int move, 
  int blackConnect, int whiteConnect, bool blackblock, bool whiteBlock, bool checkForbidden)
{
  int length = (checkForbidden ? 11 : 9);

  /* if status == black or white, set block == true*/
  switch (status[location])
  {
    case (BLACK):
      blackblock = true; break;
    case (WHITE):
      whiteBlock = true;
  }

  if ((blackblock && whiteBlock) || status[location] == BOUND || 
    location <= 0 || location >= length - 1)
  {
    if (move == 1)
    {
      /* reached leaf */

      /* set type */
      root->type[0] = typeAnalyze(status, checkForbidden, BLACK);
      root->type[1] = typeAnalyze(status, checkForbidden, WHITE);

      /* ####################################### debug */
      print(length, status, root->type);

      /* set child node to NULL*/
      for (int i = 0; i < 4; ++i)
        root->childNode[i] = NULL;

      return;
    }
    else
    {
      /* set this node to jump node*/
      root->jump = true;

      /* jump to middle of the status */
      move += 2;
      location = length / 2;

      /* reset block */
      blackblock = false; whiteBlock = false;

      /* reset connect */
      if (checkForbidden) { blackConnect  = 0; whiteConnect = 0; } 
    }
  }
  else if (checkForbidden)
    /* if check forbidden and the status == black or white, increase connect */
    switch (status[location])
    {
      case (BLACK):
        ++blackConnect; break;
      case (WHITE):
        ++whiteConnect;
    }

  /* move location*/
  location += move;
  
  root->type[0] = NULL; root->type[1] = NULL;

  /* if connect == 4, stop playing same color at this point to prevent appearing five */

  const STATUS s[4] = {BLACK, WHITE, EMPTY, BOUND};

  for (int i = 0; i < 4; ++i)
  {
    if ((i == 0 && blackConnect >= 4) || (i == 1 && whiteConnect >= 4))
    {
      root->childNode[i] = NULL;
      continue;
    }

    root->childNode[i] = (Node*)malloc(sizeof(Node));
    root->childNode[i]->jump = false;
    status[location] = s[i];
    dfs(root->childNode[i], status, location, move,
      blackConnect, whiteConnect, blackblock, whiteBlock, checkForbidden);
  }

  /* restore current location to NO_MATTER
   * note: without this line, the classification process should still work fine,
   * but will result in printing out garbage values on NO_MATTER points */
  status[location] = NO_MATTER;
}

ChessType** TypeTree::cutSameResultChild(Node *root)
{

  ChessType **currentType = NULL;

  if (root->type[0] != NULL)
    currentType = root->type;
  
  bool canCut = true;

  for (int i = 0; i < 4; ++i)
  {
    if (root->childNode[i] != NULL)
    {
      /* if this child node is not NULL, recursion and get return */
      ChessType** returnType = cutSameResultChild(root->childNode[i]);
      
      if (returnType == NULL) 
        /* if children cannot be cut, then this node also cannot be cut */
        canCut = false;
      else if (currentType == NULL)
        currentType = returnType;
      else if (currentType[0]->length != returnType[0]->length ||
        currentType[0]->life != returnType[0]->life ||
        currentType[1]->length != returnType[1]->length ||
        currentType[1]->life != returnType[1]->life)
      {
        /* if different child has different result, cannot cut this node*/
        canCut = false;
      }
    }
  }
  if (!canCut) return NULL;
  /* cut this node, free all children and set this node's type*/
  root->type[0] = currentType[0];
  root->type[1] = currentType[1];

  for (int i = 0; i < 4; ++i)
    if (root->childNode[i] != NULL)
      free(root->childNode[i]);

  return currentType;
}

ChessType** TypeTree::classify(STATUS *status, bool checkForbidden)
{
  const int length = 11;

  /* switch root */
  Node* node = (checkForbidden ? forbiddenTree_root : commonTree_root);

  for (int move = -1, start = length / 2 - 1; ; move = 1, start += 2)
    for (int checkPoint = start; ;checkPoint += move)
    {
      /* according to the status to enter the child node */
      
      node = node->childNode[status[checkPoint]];
      
      /* if reach leaf, return type */
      if (node->type[0] != NULL)
        return node->type;

      /* if reach different color, change direction */
      if (node->jump)
        break;
    }
} 

ChessType* TypeTree::typeAnalyze(STATUS *status, bool checkForbidden, STATUS color)
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
      if (status[checkPoint] != color) break;

      ++connect;
    }

  if (connect > 5)
    /* CG's length > 5, if check forbidden, return -1, else return 5*/
    return ((checkForbidden && color == BLACK) ? (new ChessType(-1, 0)) : (new ChessType(5, 0)));
  else if (connect == 5)
    /* CG's length == 5, return 5*/
    return new ChessType(5, 0);
  else
  {
    /* CG's length < 5*/

    /* play at the analize point*/
    status[length / 2] = color;

    /* try to find the left and right bound of CG
     * if it's empty, see this point as new analize point
     * make a new status array and use recursion analize the status*/
    ChessType *lType, *rType;

    for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, checkPoint = start; count < 4; ++count, checkPoint += move)
        /* if reach CG's bound*/
        if (status[checkPoint] != color)
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
            type = typeAnalyze(newStatus, checkForbidden, color);
          }
          /* if the board of CG is not empty, it means blocked*/
          else
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
      if ((checkForbidden && color == BLACK) && connect < 4)
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

void TypeTree::print(int length, STATUS *status, ChessType **type)
{
  std::cout << std::setw(5) << ++count << "(";
  /* print status array*/
  for (int i = 0; i < length; ++i)
    switch (status[i])
    {
      case 0:
        std::cout << "X"; break;
      case 1:
        std::cout << "O"; break;
      case 2:
        std::cout << " "; break;
      case 3:
        std::cout << "|"; break;
      case 4:
        std::cout << "*"; break;
      case 5:
        std::cout << "-";
    }

  std::cout << ") ";
  for (int i = 0; i < 2; i++)
  {
    std::cout << (i == 0 ? "B" : "W") << ": " << std::setw(1);

    if (type[i]->length > 0 && type[i]->length < 5)
      /* print life or dead only if length == 1, 2, 3, 4*/
      std::cout << (type[i]->life == 1 ? "L" : "D");
    else
      std::cout << " ";

    if (type[i]->length == -1)
      std::cout << "F";
    else
      std::cout << type[i]->length;

    std::cout << ( i == 0 ? ", " : "   ");
  }

  if (count % 4 == 0) std::cout << "\n";
}

void TypeTree::searchAll(Node* root, STATUS *status, int length, int location, int move)
{
  if (root->type[0] != NULL)
  {
    print(length, status, root->type);
    return;
  }

  if (root->jump)
  {
    move += 2;
    location = length / 2;
  }

  location += move;
  
  const STATUS s[4] = {BLACK, WHITE, EMPTY, BOUND};

  for (int i = 0; i < 4; i++)
  {
    if (root->childNode[i] != NULL)
    {
      status[location] = s[i];
      searchAll(root->childNode[i], status, length, location, move);
    }
  }

  status[location] = NO_MATTER;
}

int main()
{
  TypeTree::initialize();
}