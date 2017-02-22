#include <math.h>
#include <iostream>

using namespace std;

struct Style
{
  int length, life;

  Style(int _length, bool _life)
  {
    length = _length;
    life = _life;
  }
};

enum STATUS 
{
  EMPTY = (int)' ', SAME = (int)'O', DIFFERENT = (int)'X', 
  NO_MATTER = (int)'-', ANALYZE_POINT = (int)'*'
};

void print(int length, STATUS *status, Style *style);

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

  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
    for (int i = 0, n = start; i < 4; ++i, n += move)
    {
      if (status[n] != SAME)
        break;

      connect++;
    }

  if (connect > 5)
  {
    return (checkLongConnect ? (new Style(-2, 0)) : (new Style(5, 0)));
  }
  else if (connect == 5)
  {
    return new Style(5, 0);
  }
  else
  {
    status[length / 2] = SAME;

    Style *lStyle, *rStyle;
    for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start += 2)
      for (int count = 0, n = start; count < 4; ++count, n += move)
      {
        if (status[n] == EMPTY)
        {
          STATUS newStatus[length];

          for (int i = 0; i < length; ++i)
          {
            if (i == length / 2)
              newStatus[i] = ANALYZE_POINT;
            else if ((i - (length / 2 - n)) < 0 || (i - (length / 2 - n)) >= length)
              newStatus[i] = EMPTY;
            else
              newStatus[i] = status[i - (length / 2 - n)];
          }

          if (move == -1)
            lStyle = styleAnalyze(length, newStatus, checkLongConnect);
          else
            rStyle = styleAnalyze(length, newStatus, checkLongConnect);
          
          break;
        }
        else if (status[n] == DIFFERENT)
        {
          if (move == -1)
            lStyle = new Style(0, 0);
          else
            rStyle = new Style(0, 0);
          break;
        }
        
      }

    status[length / 2] = ANALYZE_POINT;

    if (lStyle->length < rStyle->length || 
      (lStyle->length == rStyle->length && (lStyle->life < rStyle->life)))
    {
      Style* temp = lStyle;
      lStyle = rStyle;
      rStyle = temp;
    }

    if (lStyle->length == 5 && rStyle->length == 5)
      return new Style(4, 1);
    else if (lStyle->length == 5)
      return new Style(4, 0);
    else
      return new Style(lStyle->length - 1, lStyle->life);

    return new Style(0, 0);
  }
}

void print(int length, STATUS *status, Style *style)
{
  for (int i = 0; i < length; ++i)
    cout << (char)status[i];

  cout << " style : " << style->length;
  if (style->length > 0 && style->length < 5)
    cout << " " << (style->life == 1 ? "life" : "dead");

  cout << endl;
}

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