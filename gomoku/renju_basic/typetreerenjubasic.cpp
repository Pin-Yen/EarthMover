#include "../chesstype.hpp"
#include "../status.hpp"
#include "virtualboardrenjubasic.hpp"
#include "evaluatorrenjubasic.hpp"
#include "typetreerenjubasic.hpp"

#include <iostream>
#include <iomanip>

#ifdef DEBUG
#include "../../objectcounter.hpp"
#endif

bool VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic::isInit = false;

VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic::Node*
 VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic::root = NULL;

void VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic::plantTree() {
  // create tree seed
  STATUS status[ANALYZE_LENGTH];
  for (int i = 0; i < ANALYZE_LENGTH; ++i)
    status[i] = EMPTY;

  // grow tree
  dfs(root, status, ANALYZE_LENGTH / 2, -1, 0, 0, false, false);
}

// Depth First Search
// parameters of the initial call should be:
// location: length / 2, move = -1, connect = 0

// connect is used to prevent already exist five
// for example : OOOOO*OOX-- ; --X  *OOOOO
//               ^^^^^               ^^^^^
void VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic::dfs(Node *node, STATUS *status, int location,
                                            int move,  int blackConnect, int whiteConnect,
                                            bool blackBlock, bool whiteBlock) {
  // if status == black or white, set block == true*/
  switch (status[location]) {
    case BLACK:
      blackBlock = true; break;
    case WHITE:
      whiteBlock = true;
  }

  if ((blackBlock && whiteBlock) || status[location] == BOUND ||
      location <= 0 || location >= ANALYZE_LENGTH - 1) {
    if (move == 1) {
      // reached leaf

      // set type
      SingleType bType = typeAnalyze(status, BLACK, true),
                 wType = typeAnalyze(status, WHITE, true);
      node->type = ChessType(bType, wType);

      node->leaf = true;

      return;
    } else {
      // set this node to jump node
      node->jump = true;

      // jump to middle of the status
      move += 2;
      location = ANALYZE_LENGTH / 2;

      // reset block
      blackBlock = false; whiteBlock = false;

      // reset connect
      blackConnect = 0; whiteConnect = 0;
    }
  } else {
    // if status == black or white, increase connect
    switch (status[location]) {
      case BLACK:
        ++blackConnect; break;
      case WHITE:
        ++whiteConnect;
    }
  }

  // move location
  location += move;

  const STATUS s[4] = {BLACK, WHITE, EMPTY, BOUND};

  for (int i = 0; i < 4; ++i) {
    // if connect == 4, stop playing same color at this point to prevent appearing five
    if ((i == 0 && blackConnect >= 4) || (i == 1 && whiteConnect >= 4))
      continue;

    node->childNode[i] = new Node();
    status[location] = s[i];
    dfs(node->childNode[i], status, location, move,
        blackConnect, whiteConnect, blackBlock, whiteBlock);
  }

  // restore current location to EMPTY
  // note: without this line, the classification process should still work fine,
  // but will result in printing out garbage values on EMPTY points
  status[location] = EMPTY;
}

SingleType VirtualBoardRenjuBasic::EvaluatorRenjuBasic::TypeTreeRenjuBasic::typeAnalyze(
    STATUS *status, STATUS color, bool checkLevel) {
  int connect = 1;
  // check the length of the connection around the analize point
  // under the following, we call this chess group "center group" (CG)
  // for example: --X O*OOX-- ; OOOO* O X
  //                  ^^^^      ^^^^^
  for (int move = -1, start = ANALYZE_LENGTH / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, checkPoint = start; i < 4; ++i, checkPoint += move) {
      if (status[checkPoint] != color) break;

      ++connect;
    }

  if (connect > 5) {
    // CG's length > 5, return -1
    if (color == BLACK)
      return SingleType(-1, 0, 0);
    else
      return SingleType(5, 0, 0);
  } else if (connect == 5) {
    // CG's length == 5, return 5
    return SingleType(5, 0, 0);
  } else {
    // CG's length < 5

    // play at the analize point
    status[ANALYZE_LENGTH / 2] = color;

    // try to find the left and right bound of CG
    // if it's empty, see this point as new analize point
    // make a new status array and use recursion analize the status
    SingleType lType, rType;
    bool lInit = false, rInit = false;
    int level = 0;

    for (int move = -1, start = ANALYZE_LENGTH / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, checkPoint = start; count < 4; ++count, checkPoint += move)
        // if reach CG's bound
        if (status[checkPoint] != color) {
          SingleType type;
          bool blocked = false;

          // if the bound is an empty point
          if (status[checkPoint] == EMPTY) {
            // make a new status array
            STATUS newStatus[ANALYZE_LENGTH];

            // transform from origin status
            for (int i = 0; i < ANALYZE_LENGTH; ++i) {
              int transformation_index = i - (ANALYZE_LENGTH / 2 - checkPoint);

              if (transformation_index < 0 || transformation_index >= ANALYZE_LENGTH)
                  // if out of bound, set it to Bound
                newStatus[i] = BOUND;
              else
                newStatus[i] = status[transformation_index];
            }

            // recursion analize
            type = typeAnalyze(newStatus, color, false);
          } else {
            // if the board of CG is not empty, it means blocked
            blocked = true;
            type = SingleType(0, 0, 0);
          }

          if (move == -1) {
            // left type result
            if (!lInit) {
              lInit = true;
              lType = type;
              if (!checkLevel || blocked) break;
              if (!lType.life() || lType.length() > 3) break;
            } else {
              if (lType == type) {
                ++level;
              } else {
                break;
              }
            }
          } else {
            // right type result
            if (!rInit) {
              rInit = true;
              rType = type;
              if (!checkLevel || blocked) break;
              if (!rType.life() || rType.length() > 3) break;
              if (lType == rType) {
                ++level;
              } else if (lType < rType) {
                level = 0;
              } else {
                break;
              }
            } else {
              if ((rType == type) && (rType >= lType)) {
                ++level;
              } else {
                break;
              }
            }
          }
        }

    // restore the analize point to empty
    status[ANALYZE_LENGTH / 2] = EMPTY;

    // keep lType > rType
    if (lType < rType)
      std::swap(lType, rType);

    if (lType.length() == 5 && rType.length() == 5) {
      // if left and right will both produce 5 after play at analize point
      if (connect == 4)
        // if connect == 4, it is a life four type
        return SingleType(4, 1, 0);
      else
        // it is a forbidden (X X*X X)
        return SingleType(-1, 0, 0);
    } else if (lType.length() == 5) {
      // if there is only one side produces 5 after play at analize point,
      // it is a dead four type
      return SingleType(4, 0, 0);
    } else if (lType.length() <= 0) {
      // if the longer size produces 0 or forbidden point after play at analize point,
      // it is a useless point
      return SingleType(0, 0, 0);
    } else {
      // if left length < 4, return left length - 1
      // (current recursion's result = lower recursion's result - 1)
      if (checkLevel) {
        if (!lType.life() || lType.length() > 3) {
          return SingleType(lType.length() - 1, lType.life(), 0);
        } else {
          return SingleType(lType.length() - 1, lType.life(),
                           level - (3 - (lType.length() - 1)));
        }
      } else {
        return SingleType(lType.length() - 1, lType.life(), 0);
      }
    }
  }
}