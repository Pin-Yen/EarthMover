#include <math.h>
#include <iostream>

using namespace std;

enum STATUS {EMPTY = (int)' ', SAME = (int)'O', DIFFERENT = (int)'X', NO_MATTER = (int)'-'};

void styleMaker(int length, int index, STATUS* status)
{
  for (int i = 0; i < length; ++i)
  {
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
  for (int move = -1, start = length / 2 - 1; move <= 1; move += 2, start++)
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

void print(int length, STATUS *status)
{
  for (int i = 0; i < length; ++i)
  {
    if (i == length / 2)
      cout << '*';

    cout << (char)status[i];
  }

  cout << "\n";
}

int main()
{

  const int length = 10;
  int styleAmount = pow(3, length);

  for (int i = 0, n = 0; i < styleAmount; ++i)
  {
    STATUS status[length];
    styleMaker(length, i, status);
    if (checkNecessary(length, status))
    {
      n++;
      cout << n << " : "; 
      print(length, status);

    }
  }
}