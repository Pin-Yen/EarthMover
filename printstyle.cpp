#include <math.h>
#include <iostream>

using namespace std;

struct Style
{
  int length, life;

  Style(int le, bool li)
  {
    length = le;
    life = li;
  }
};

enum STATUS 
{
  EMPTY = (int)' ', SAME = (int)'O', DIFFERENT = (int)'X', 
  NO_MATTER = (int)'-', ANALYZE_POINT = (int)'*'
};

void styleMaker(int length, int index, STATUS *status);

bool checkNecessary(int length, STATUS *status);

Style * styleAnalyze(int length, STATUS *status, bool checkLongConnect);

void print(int length, STATUS *status, Style *style);

int main()
{

  cout << "not considering long connect : \n";
  int length = 9;
  int styleAmount = pow(3, length - 1);

  for (int i = 0, n = 0; i < styleAmount; ++i)
  {
    STATUS status[length];
    styleMaker(length, i, status);
    if (checkNecessary(length, status))
    {
      n++;
      cout << n << " : "; 

      Style* style = styleAnalyze(length, status, false);

      print(length, status, style);
    }
  }

  cout << "\nconsidering long connect : \n";
  length = 11;
  styleAmount = pow(3, length - 1);

  for (int i = 0, n = 0; i < styleAmount; ++i)
  {
    STATUS status[length];
    styleMaker(length, i, status);
    if (checkNecessary(length, status))
    {
      n++;
      cout << n << " : "; 

      Style* style = styleAnalyze(length, status, true);

      print(length, status, style);
    }
  }
}

void styleMaker(int length, int index, STATUS *status)
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
   
  return true; 
}

Style * styleAnalyze(int length, STATUS *status, bool checkLongConnect)
{
  int connect = 1;

  /* check the length of the connection around the analize point*/
  /* under the following, we call this chess group "center group" (CG)*/
  /* for example : --X O*OOX-- ; OOOO* O X */
  /*                    ^^^^      ^^^^^     */
  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, n = start; i < 4; ++i, n += move)
    {
      if (status[n] != SAME)
        break;

      connect++;
    }

  if (connect > 5)
    /* CG's length > 5, if check long connect, return -2, else return 5*/
    return (checkLongConnect ? (new Style(-2, 0)) : (new Style(5, 0)));
  else if (connect == 5)
    /* CG's length == 5, return 5*/
    return new Style(5, 0);
  else
  {
    /* CG's length < 5*/

    /* play at the analize point*/
    status[length / 2] = SAME;

    /* try to find the left and right bound of CG*/
    /* if it's empty, see this point as new analize point*/
    /* make a new status array and use recursion analize the status*/
    Style *lStyle, *rStyle;

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
            lStyle = styleAnalyze(length, newStatus, checkLongConnect);
          else
            rStyle = styleAnalyze(length, newStatus, checkLongConnect);
          
          break;
        }
        /* if the board of CG is opponent chess, it means blocked*/
        else if (status[n] == DIFFERENT)
        {
          if (move == -1)
            lStyle = new Style(0, 0);
          else
            rStyle = new Style(0, 0);
          break;
        }
        
      }

    /* restore the analize point */
    status[length / 2] = ANALYZE_POINT;

    /* keep lStyle > rStyle*/
    if (lStyle->length < rStyle->length || 
      (lStyle->length == rStyle->length && (lStyle->life < rStyle->life)))
    {
      Style* temp = lStyle;
      lStyle = rStyle;
      rStyle = temp;
    }

    if (lStyle->length == 5 && rStyle->length == 5)
      /* if left and right will both produce 5 after play at analize point,*
      /* it is a life four style*/
      return new Style(4, 1);
    else if (lStyle->length == 5)
      /* if left and right only one side produce 5 after play at analize point,*
      /* it is a dead four style*/
      return new Style(4, 0);
    else
      /* if left length < 4, return left length - 1*/
      /* (current recursion's result = lower recursion's result - 1) */
      return new Style(lStyle->length - 1, lStyle->life);
  }
}

void print(int length, STATUS *status, Style *style)
{
  for (int i = 0; i < length; ++i)
    cout << (char)status[i];

  cout << " style : " << style->length;

  if (style->length > 0 && style->length < 5)
    cout << " " << (style->life == 1 ? "life" : "dead");

  cout << "\n";
}