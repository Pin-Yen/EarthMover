#ifndef STYLE_H
#define STYLE_H

struct Style
{
  int length, life;

  Style(int length, bool life)
  {
    this->length = length;
    this->life = life;
  }
};

#endif