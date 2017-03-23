#include "chesstype.hpp"
#include "status.hpp"
#include "typetree.hpp"
#include "../../objectcounter.hpp"

#include <iostream>
#include <iomanip>

/* initialize root*/
TypeTree::Node* TypeTree::root = new Node();

void TypeTree::initialize() {
  /* initialize status*/
  STATUS status[analyze_length];
  for (int i = 0; i < analyze_length; ++i)
    status[i] = EMPTY;

  dfs(root, status, analyze_length / 2, -1, false, false);

  cutSameResultChild(root);
}

/* Depth First Search
 * parameters of the initial call should be:
 * location: length / 2, move = -1, connect = 0 */

/* connect is used to prevent already exist five while length == 11
 * for example : OOOOO*OOX-- ; --X  *OOOOO
 *               ^^^^^               ^^^^^ */
void TypeTree::dfs(Node *root, STATUS *status, int location, int move,
                   bool blackBlock, bool whiteBlock) {
  /* if status == black or white, set block == true*/
  switch (status[location]) {
    case BLACK:
      blackBlock = true; break;
    case WHITE:
      whiteBlock = true;
  }

  if ((blackBlock && whiteBlock) || status[location] == BOUND ||
      location <= 0 || location >= analyze_length - 1) {
    if (move == 1) {
      /* reached leaf */

      /* set type */
      root->type[0] = typeAnalyze(status, BLACK);
      root->type[1] = typeAnalyze(status, WHITE);

      /* set child node to NULL*/
      for (int i = 0; i < 4; ++i)
        root->childNode[i] = NULL;

      return;
    } else {
      /* set this node to jump node */
      root->jump = true;

      /* jump to middle of the status */
      move += 2;
      location = analyze_length / 2;

      /* reset block */
      blackBlock = false; whiteBlock = false;
    }
  }

  /* move location */
  location += move;

  root->type[0] = NULL; root->type[1] = NULL;

  const STATUS s[4] = {BLACK, WHITE, EMPTY, BOUND};

  for (int i = 0; i < 4; ++i) {
    root->childNode[i] = new Node();
    status[location] = s[i];
    dfs(root->childNode[i], status, location, move, blackBlock, whiteBlock);
  }

  /* restore current location to EMPTY
   * note: without this line, the classification process should still work fine,
   * but will result in printing out garbage values on EMPTY points */
  status[location] = EMPTY;
}

ChessType** TypeTree::cutSameResultChild(Node *root) {
  ChessType **currentType = NULL;

  if (root->type[0] != NULL) {
    currentType = root->type;
    return currentType;
  }

  bool canCut = true;

  for (int i = 0; i < 4; ++i)
    if (root->childNode[i] != NULL) {
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
        /* if different child has different result, cannot cut this node*/
        canCut = false;
    }

  if (!canCut) return NULL;
  /* cut this node, free all children and set this node's type */
  root->type[0] = new ChessType(currentType[0]);
  root->type[1] = new ChessType(currentType[1]);

  for (int i = 0; i < 4; ++i)
    if (root->childNode[i] != NULL)
      delete root->childNode[i];

  return currentType;
}

void TypeTree::classify(STATUS *status, ChessType *(type[2])) {
  /* switch root */
  Node* node = root;

  for (int move = -1, start = classify_length / 2 - 1; ; move = 1, ++start)
    for (int checkPoint = start; ; checkPoint += move) {
      /* according to the status to enter the child node */
      node = node->childNode[status[checkPoint]];

      /* if reach leaf, return type */
      if (node->type[0] != NULL) {
        if (type[0] != NULL) delete type[0];
        if (type[1] != NULL) delete type[1];
        type[0] = new ChessType(node->type[0]);
        type[1] = new ChessType(node->type[1]);

        return;
      }

      /* if reach different color, change direction */
      if (node->jump)
        break;
    }
}

ChessType* TypeTree::typeAnalyze(STATUS *status, STATUS color) {
  int connect = 1;

  /* check the length of the connection around the analize point
   * under the following, we call this chess group "center group" (CG)
   * for example: --X O*OOX-- ; OOOO* O X
   *                   ^^^^      ^^^^^     */
  for (int move = -1, start = analyze_length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, checkPoint = start; i < 4; ++i, checkPoint += move) {
      if (status[checkPoint] != color) break;

      ++connect;
    }

  if (connect >= 5)
    /* CG's length == 5, return 5 */
    return new ChessType(5, 0);
  else {
    /* CG's length < 5 */

    /* play at the analize point */
    status[analyze_length / 2] = color;

    /* try to find the left and right bound of CG
     * if it's empty, see this point as new analize point
     * make a new status array and use recursion analize the status */
    ChessType *lType, *rType;

    for (int move = -1, start = analyze_length / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, checkPoint = start; count < 4; ++count, checkPoint += move)
        /* if reach CG's bound */
        if (status[checkPoint] != color) {
          ChessType* type;

          /* if the bound is an empty point */
          if (status[checkPoint] == EMPTY) {
            /* make a new status array */
            STATUS newStatus[analyze_length];

            /* transform from origin status */
            for (int i = 0; i < analyze_length; ++i) {
              int transformation_index = i - (analyze_length / 2 - checkPoint);

              if (transformation_index < 0 || transformation_index >= analyze_length)
                  /* if out of bound, see it as empty point */
                newStatus[i] = EMPTY;
              else
                newStatus[i] = status[transformation_index];
            }

            /* recursion analize */
            type = typeAnalyze(newStatus, color);

          } else {
            /* if the board of CG is not empty, it means blocked */
            type = new ChessType(0, 0);
          }

          /* set analize result l/rType */
          if (move == -1)
            lType = type;
          else
            rType = type;

          break;
        }

    /* restore the analize point to empty */
    status[analyze_length / 2] = EMPTY;

    /* keep lType > rType */
    if ((lType->length < rType->length) ||
        ((lType->length == rType->length) && (lType->life < rType->life)))
      std::swap(lType, rType);

    ChessType *returnType;

    if (lType->length == 5 && rType->length == 5)
      /* if left and right will both produce 5 after play at analize point
       * it is a life four type */
      returnType =  new ChessType(4, 1);
    else if (lType->length == 5)
      /* if there is only one side produces 5 after play at analize point,
       * it is a dead four type */
      returnType = new ChessType(4, 0);
    else if (lType->length == 0)
      /* if the longer size produces 0 or forbidden point after play at analize point,
       * it is a useless point */
      returnType = new ChessType(0, 0);
    else
      /* if left length < 4, return left length - 1
       * (current recursion's result = lower recursion's result - 1) */
      returnType = new ChessType(lType->length - 1, lType->life);

    delete lType;
    delete rType;

    return returnType;
  }
}

void TypeTree::print(STATUS *status, ChessType **type) {
  std::cout << std::setw(5) << "(";
  /* print status array*/
  for (int i = 0; i < analyze_length; ++i) {
    char c;
    if (i == analyze_length / 2)
      c = '*';
    else
      switch (status[i]) {
        case 0:
          c = 'X'; break;
        case 1:
          c = 'O'; break;
        case 2:
          c = ' '; break;
        case 3:
          c = '|';
      }

    std::cout << c;
  }


  std::cout << ") ";
  for (int i = 0; i < 2; i++) {
    std::cout << (i == 0 ? "B" : "W") << ":" << std::setw(1);

    if (type[i]->length > 0)
      /* print life or dead only if length == 1, 2, 3, 4*/
      std::cout << (type[i]->life == 1 ? " L" : " D") << type[i]->length;
    else
      std::cout << "  " << type[i]->length;

    std::cout << ( i == 0 ? ", " : "   ");
  }

  std::cout << "\n";
}

void TypeTree::searchAll(Node* root, STATUS *status, int location, int move) {
  if (root->type[0] != NULL) {
    // print(status, root->type);
    return;
  }

  if (root->jump) {
    move += 2;
    location = analyze_length / 2;
  }

  location += move;

  const STATUS s[4] = {BLACK, WHITE, EMPTY, BOUND};

  for (int i = 0; i < 4; i++)
    if (root->childNode[i] != NULL) {
      status[location] = s[i];
      searchAll(root->childNode[i], status, location, move);
    }

  status[location] = EMPTY;
}
