#include "openingtree.hpp"

#include <assert.h>
#include <iostream>

/* initialize root*/
OpeningTree::Node* OpeningTree::root = new Node();

void OpeningTree::initialize() {
  int tables[8][7][7] = {{{0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 1, 0, 2, 0, 0, 0},  /*   X   O       */
                          {0, 0, 2, 0, 0, 0, 0},  /*     O         */
                          {0, 5, 0, 1, 0, 0, 0},  /*   C   X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               1*/

                         {{0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 1, 0, 0, 0, 0, 0},  /*   X           */
                          {0, 5, 2, 2, 5, 0, 0},  /*   C O O C     */
                          {0, 0, 5, 1, 0, 0, 0},  /*     C X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               2*/

                         {{0, 0, 2, 0, 0, 0, 0},  /*     O         */
                          {0, 1, 0, 0, 0, 0, 0},  /*   X           */
                          {0, 0, 2, 0, 0, 0, 0},  /*     O         */
                          {0, 4, 4, 1, 0, 0, 0},  /*   B B X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               3*/

                         {{0, 0, 0, 2, 0, 0, 0},  /*       O       */
                          {0, 1, 0, 0, 0, 0, 0},  /*   X           */
                          {0, 0, 2, 0, 0, 0, 0},  /*     O         */
                          {0, 4, 4, 1, 0, 0, 0},  /*   B B X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               4*/

                         {{0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 1, 0, 0, 0, 0},  /*     X         */
                          {0, 0, 2, 2, 3, 0, 0},  /*     O O A     */
                          {0, 4, 0, 1, 0, 0, 0},  /*   B   X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               5*/

                         {{0, 0, 0, 0, 3, 0, 0},  /*         B     */
                          {0, 0, 1, 2, 0, 0, 0},  /*     X O       */
                          {0, 0, 2, 0, 0, 0, 0},  /*     O         */
                          {0, 4, 0, 1, 0, 0, 0},  /*   A   X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               6*/

                         {{0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 1, 3, 0, 0, 0},  /*     X A       */
                          {0, 0, 2, 3, 0, 0, 0},  /*     O A       */
                          {0, 2, 0, 1, 0, 0, 0},  /*   O   X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}, /*               7*/

                         {{0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 1, 0, 0, 0, 0},  /*     X         */
                          {0, 2, 2, 3, 0, 0, 0},  /*   O O A       */
                          {0, 3, 0, 1, 0, 0, 0},  /*   A   X       */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0},  /*               */
                          {0, 0, 0, 0, 0, 0, 0}}};/*               8*/

  for (int i = 0; i < 1; ++i) {
    for (int r = 0; r < 4; ++r) {
      insert(tables[i]);
      rotate(tables[i]);
    }
    mirror(tables[i]);
    for (int r = 0; r < 4; ++r) {
      insert(tables[i]);
      rotate(tables[i]);
    }
  }
}

void OpeningTree::rotate(int table[7][7]) {
  int temp[7][7];
  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c)
      temp[c][7 - r] = table[r][c];

  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c)
      table[r][c] = temp[r][c];
}

void OpeningTree::mirror(int table[7][7]) {
  int temp[7][7];
  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c)
      temp[c][r] = table[r][c];

  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c)
      table[r][c] = temp[r][c];
}

void OpeningTree::insert(int table[7][7]) {
  Node* currentNode = root;

  int oriRow = 0, oriCol = 0, curRow, curCol, color;

  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c)
      if (table[r][c] > 0 && table[r][c] < 3) {
        if (r > oriRow)
          oriRow = r;
        if (c > oriCol)
          oriCol = c;
      }

  curRow = oriRow; curCol = oriCol;

  while (true) {
    --curCol;
    if (curCol < 0) {
      if (curRow == 0) break;
      --curRow;

      curCol = 6;
    }

    if (table[curRow][curCol] > 0 && table[curRow][curCol] < 3) {
      color = table[curRow][curCol] - 1;
      if (currentNode->childNode[oriRow - curRow][oriCol - curCol][color] == NULL)
        currentNode->childNode[oriRow - curRow][oriCol - curCol][color] = new Node();
      currentNode = currentNode->childNode[oriRow - curRow][oriCol - curCol][color];
    }
  }

  for (curRow = 0; curRow < 7; ++curRow)
    for (curCol = 0; curCol < 7; ++curCol)
      if (table[curRow][curCol] >= 3) {
        if (currentNode->childNode[oriRow - curRow + 3][oriCol - curCol + 3][0] == NULL)
          currentNode->childNode[oriRow - curRow + 3][oriCol - curCol + 3][0] = new Node();
        else
          continue;
        currentNode->childNode[oriRow - curRow + 3][oriCol - curCol + 3][0]->score =
          table[curRow][curCol];
      }
}

void OpeningTree::classify(int table[7][7]) {
  Node* currentNode = root;

  int oriRow = 0, oriCol = 0, curRow, curCol, color;

  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c)
      if (table[r][c] > 0 && table[r][c] < 3) {
        if (r > oriRow)
          oriRow = r;
        if (c > oriCol)
          oriCol = c;
      }

  curRow = oriRow; curCol = oriCol;

  while (true) {
    --curCol;
    if (curCol < 0) {
      if (curRow == 0) break;
      --curRow;

      curCol = 6;
    }

    if (table[curRow][curCol] > 0 && table[curRow][curCol] < 3) {
      color = table[curRow][curCol] - 1;
      if (currentNode->childNode[oriRow - curRow][oriCol - curCol][color] == NULL) return;

      currentNode = currentNode->childNode[oriRow - curRow][oriCol - curCol][color];
    }
  }

  for (int curRow = 0; curRow < 7; ++curRow)
    for (int curCol = 0; curCol < 7; ++curCol)
      if (currentNode->childNode[curRow][curCol][0] != NULL)
        std::cout << curCol - 3 << ", "
                  << curRow - 3 << " :"
                  << currentNode->childNode[curRow][curCol][0]->score
                  << std::endl;
}

OpeningTree::Node::Node() {
  for (int r = 0; r < 7; ++r)
    for (int c = 0; c < 7; ++c) {
      childNode[r][c][0] = NULL;
      childNode[r][c][1] = NULL;
    }
}

int main()
{
  OpeningTree::initialize();

  int test[7][7] = {{0, 0, 0, 0, 0, 0, 0},  /*               */
                    {0, 0, 0, 0, 0, 0, 0},  /*               */
                    {0, 0, 0, 0, 0, 0, 0},  /*               */
                    {0, 1, 0, 2, 0, 0, 0},  /*  X   O        */
                    {0, 0, 2, 0, 0, 0, 0},  /*    O          */
                    {0, 0, 0, 1, 0, 0, 0},  /*      X        */
                    {0, 0, 0, 0, 0, 0, 0}}; /*               */

  int tes2[7][7] = {{0, 0, 0, 0, 0, 0, 0},  /*               */
                    {0, 0, 0, 0, 0, 0, 0},  /*               */
                    {0, 0, 0, 0, 0, 0, 0},  /*               */
                    {0, 2, 0, 1, 0, 0, 0},  /*  O   X        */
                    {0, 0, 2, 0, 0, 0, 0},  /*    O          */
                    {0, 1, 0, 0, 0, 0, 0},  /*  X            */
                    {0, 0, 0, 0, 0, 0, 0}}; /*               */

  OpeningTree::classify(tes2);
}