#include "openingtree.hpp"

#include <iostream>
#include <fstream>

/* initialize root*/
OpeningTree::Node* OpeningTree::root = new Node();

void OpeningTree::initialize() {
  std::ifstream file("opening.txt");

  int openingAmout;
  while (file >> openingAmout) {
    std::cout << "inserting opening " << openingAmout << std::endl;

    char table[5][5];
    for (int r = 0; r < 5; ++r)
      for (int c = 0; c < 5; ++c)
        file >> table[r][c];

    /* insert 8 direction
       rotate four times -> mirror -> rotate four times */
    for (int m = 0; m < 2; ++m) {
      for (int r = 0; r < 4; ++r) {
        insert(table);
        rotate(table);
      }
      mirror(table);
    }
  }

  std::cout << "finished insert " << openingAmout << " opening table" << std::endl;
  file.close();
}

void OpeningTree::rotate(char table[5][5]) {
  char temp[5][5];
  /* rotate 90 degrees clockwise (row -> col, col -> 4 - row) */
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      temp[c][4 - r] = table[r][c];

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      table[r][c] = temp[r][c];
}

void OpeningTree::mirror(char table[5][5]) {
  char temp[5][5];
  /* mirror (row -> col, col -> row) */
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      temp[c][r] = table[r][c];

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      table[r][c] = temp[r][c];
}

void OpeningTree::insert(char table[5][5]) {
  Node* currentNode = root;

  int oriRow = 0, oriCol = 0, curRow, curCol, color;

  /* set the origin in the lower right corner in the table
     e.g. the origin of the example below is "0"
          /*   X        */
          /*   C O O C  */
          /*     C X 0  */
          /*            */
          /*            */
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      if (table[r][c] == 'X' || table[r][c] == 'O') {
        if (r > oriRow)
          oriRow = r;
        if (c > oriCol)
          oriCol = c;
      }

  /* set current point to origin */
  curRow = oriRow; curCol = oriCol;

  while (true) {
    /* the insert order is from right to left, from bottom to top */
    --curCol;
    if (curCol < 0) {
      if (curRow == 0) break;
      --curRow;

      curCol = 4;
    }

    /* if find a occupied point */
    if (table[curRow][curCol] == 'X' || table[curRow][curCol] == 'O') {
      color = table[curRow][curCol] == 'X' ? 0 : 1;

      /* using vector type to record (origin to current point) */
      if (currentNode->childNode[oriRow - curRow][oriCol - curCol][color] == NULL)
        currentNode->childNode[oriRow - curRow][oriCol - curCol][color] = new Node();
      currentNode = currentNode->childNode[oriRow - curRow][oriCol - curCol][color];
    }
  }

  /* record the fifth move */
  for (curRow = 0; curRow < 5; ++curRow)
    for (curCol = 0; curCol < 5; ++curCol)
      if (table[curRow][curCol] == 'P') {
        /* using vector type to record
           to avoid out of bound (vector may be negative), all value ++ */
        if (currentNode->childNode[oriRow - curRow + 1][oriCol - curCol + 1][0] == NULL)
          currentNode->childNode[oriRow - curRow + 1][oriCol - curCol + 1][0] = new Node();
      }
}

/* classify method please refer to insert */
void OpeningTree::classify(char table[5][5]) {
  Node* currentNode = root;

  int oriRow = 0, oriCol = 0, curRow, curCol, color;

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      if (table[r][c] == 'X' || table[r][c] == 'O') {
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

      curCol = 4;
    }

    if (table[curRow][curCol] == 'X' || table[curRow][curCol] == 'O') {
      color = table[curRow][curCol] == 'X' ? 0 : 1;
      if (currentNode->childNode[oriRow - curRow][oriCol - curCol][color] == NULL) return;

      currentNode = currentNode->childNode[oriRow - curRow][oriCol - curCol][color];
    }
  }

  for (int curRow = 0; curRow < 5; ++curRow)
    for (int curCol = 0; curCol < 5; ++curCol)
      if (currentNode->childNode[curRow][curCol][0] != NULL)
        // debug purpose, print result
        std::cout << curCol - 1 << ", " << curRow - 1 << std::endl;
}

int main()
{
  OpeningTree::initialize();
  std::ifstream file("openingtest.txt");

  int testAmount;
  while (file >> testAmount) {
    char table[5][5];
    for (int r = 0; r < 5; ++r) {
      for (int c = 0; c < 5; ++c) {
        file >> table[r][c];
        std::cout << table[r][c];
      }
      std::cout << std::endl;
    }

    OpeningTree::classify(table);
    std::cout << std::endl;
  }

  file.close();
}