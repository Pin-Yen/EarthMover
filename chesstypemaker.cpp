#include <math.h>
#include <iostream>
#include "chesstype.hpp"

using namespace std;

enum STATUS 
{
  EMPTY = (int)' ', SAME = (int)'O', DIFFERENT = (int)'X', 
  NO_MATTER = (int)'-', ANALYZE_POINT = (int)'*'
};

void typeMaker(int length, int index, STATUS *status);

bool checkNecessary(int length, STATUS *status);

ChessType * typeAnalyze(STATUS *status, bool checkForbidden);

void print(int length, STATUS *status, ChessType *type);

int main()
{
  for (int length = 9; length <= 11; length += 2)
  {
    cout << (length == 9 ? 
      "not considering long connect : \n" : "considering long connect : \n");

    int styleAmount = pow(3, length - 1);

    for (int i = 0, n = 0; i < styleAmount; ++i)
    {
      STATUS status[length];
      typeMaker(length, i, status);

      if (checkNecessary(length, status))
      {
        cout << ++n << ": "; 

        bool checkForbidden = !(length == 9);

        ChessType* type = typeAnalyze(status, checkForbidden);

        print(length, status, type);
      }
    }
  }
}


void typeMaker(int length, int index, STATUS *status)
{
  for (int i = 0; i < length; ++i)
  {
    if (i == length / 2) 
    {
      status[i] = ANALYZE_POINT;
      continue;
    }

    switch (index % 3)
    {
      case 0:
        status[i] = EMPTY; break;
      case 1:
        status[i] = SAME; break;
      default:
        status[i] = DIFFERENT;
    }

    index /= 3;
  }
}

bool checkNecessary(int length, STATUS *status)
{
  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
  {
    bool meetDiff = false;
    for (int i = 0, n = start; i < length / 2; ++i, n += move)
    {
      if (meetDiff)
      {
        if (status[n] == EMPTY)
          status[n] = NO_MATTER;
        else
          return false;
      }

      if (status[n] == DIFFERENT)
        meetDiff = true;
    }
    meetDiff = false;
  }

  if (length == 11)
    for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    {  
      int length = 0;
      for (int i = 0, n = start; i < 5; ++i, n += move)
      {
        if (status[n] != SAME) break;

        ++length;

        if (length == 5) return false;
      }
    }
   
  return true; 
}

ChessType * typeAnalyze(STATUS *status, bool checkForbidden)
{
  int length = checkForbidden ? 11 : 9;

  int connect = 1;

  /* check the length of the connection around the analize point*/
  /* under the following, we call this chess group "center group" (CG)*/
  /* for example : --X O*OOX-- ; OOOO* O X */
  /*                   ^^^^      ^^^^^     */
  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, n = start; i < 4; ++i, n += move)
    {
      if (status[n] != SAME) break;

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

    /* try to find the left and right bound of CG*/
    /* if it's empty, see this point as new analize point*/
    /* make a new status array and use recursion analize the status*/
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
      /* if there is only one side produce 5 after play at analize point,*/
      /* it is a dead four style*/
      return new ChessType(4, 0);
    else if (lType->length == 0 || lType-> length == -1)
      /* if the longer size produce 6 or 0 after play at analize point,*/
      /* it is a useless point*/
      return new ChessType(0, 0);
    else
      /* if left length < 4, return left length - 1*/
      /* (current recursion's result = lower recursion's result - 1) */
      return new ChessType(lType->length - 1, lType->life);
  }
}

void print(int length, STATUS *status, ChessType *type)
{
  /* print status array*/
  for (int i = 0; i < length; ++i)
    cout << (char)status[i];

  cout << " type: ";

  if (type->length > 0 && type->length < 5)
    /* print life or dead only if length == 1, 2, 3, 4*/
    cout << (type->life == 1 ? "life" : "dead") << " ";

  if (type->length == -1)
    cout << "forbidden point";
  else
    cout << type->length;

  cout << endl;
}