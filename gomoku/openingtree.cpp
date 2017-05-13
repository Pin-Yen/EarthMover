#include "status.hpp"
#include "freestyle/virtualboard.hpp"
#include "freestyle/point.hpp"
#include "freestyle/evaluator.hpp"
#include "openingtree.hpp"

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>

/* initialize root*/
VirtualBoard::Evaluator::OpeningTree::Node* VirtualBoard::Evaluator::OpeningTree::root = new Node();

void VirtualBoard::Evaluator::OpeningTree::initialize() {
  std::ifstream file("gomoku/opening.txt");

  float openingAmout;
  while (file >> openingAmout) {
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

  file.close();
}

void VirtualBoard::Evaluator::OpeningTree::rotate(char table[5][5]) {
  char temp[5][5];
  /* rotate 90 degrees clockwise (row -> col, col -> 4 - row) */
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      temp[c][4 - r] = table[r][c];

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      table[r][c] = temp[r][c];
}

void VirtualBoard::Evaluator::OpeningTree::mirror(char table[5][5]) {
  char temp[5][5];
  /* mirror (row -> col, col -> row) */
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      temp[c][r] = table[r][c];

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      table[r][c] = temp[r][c];
}

void VirtualBoard::Evaluator::OpeningTree::insert(char table[5][5]) {
  Node* currentNode = root;

  int oriRow = 0, oriCol = 0, curRow, curCol;
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
    /* if find a occupied point */
    if (table[curRow][curCol] == 'X' || table[curRow][curCol] == 'O') {
      int color = table[curRow][curCol] == 'X' ? 0 : 1;

      /* using vector type to record (origin to current point) */
      if (currentNode->childNode[oriRow - curRow][oriCol - curCol][color] == NULL)
        currentNode->childNode[oriRow - curRow][oriCol - curCol][color] = new Node();
      currentNode = currentNode->childNode[oriRow - curRow][oriCol - curCol][color];
    }

    /* the insert order is from right to left, from bottom to top */
    --curCol;
    if (curCol < 0) {
      if (curRow == 0) break;
      --curRow;

      curCol = 4;
    }
  }

  /* record the fifth move */
  for (curRow = 0; curRow < 5; ++curRow)
    for (curCol = 0; curCol < 5; ++curCol)
      if (table[curRow][curCol] == 'P') {
        /* using vector type to record */
        std::array<int, 2> result = {oriRow - curRow, oriCol - curCol};
        /* if vector didnot contain this result, add this result into vector */
        if (std::find(currentNode->result.begin(), currentNode->result.end(), result) ==
            currentNode->result.end())
          currentNode->result.push_back(result);
      }
}

/* classify method please refer to insert */
void VirtualBoard::Evaluator::OpeningTree::classify(VirtualBoard::Point* point[15][15],
                                                    int *row, int *col) {
  Node* currentNode = root;

  int oriRow = 0, oriCol = 0, curRow, curCol;
  int left = 14, top = 14;
  /* set the origin in the lower right corner, find the left and top */
  for (int r = 0; r < 15; ++r)
    for (int c = 0; c < 15; ++c)
      if (point[r][c]->status() == BLACK || point[r][c]->status() == WHITE) {
        if (r < top)
          top = r;
        if (c < left)
          left = c;
        if (r > oriRow)
          oriRow = r;
        if (c > oriCol)
          oriCol = c;
      }

  if (oriRow - top > 4 || oriCol - left > 4) return;

  curRow = oriRow; curCol = oriCol;

  while (true) {
    if (point[curRow][curCol]->status() == BLACK || point[curRow][curCol]->status() == WHITE) {
      STATUS color = point[curRow][curCol]->status();
      if (currentNode->childNode[oriRow - curRow][oriCol - curCol][color] == NULL) return;

      currentNode = currentNode->childNode[oriRow - curRow][oriCol - curCol][color];
    }

    --curCol;
    if (curCol < oriCol - 4 || curCol < 0) {
      if (curRow == oriRow - 4 || curRow == 0) break;
      --curRow;

      curCol = oriCol;
    }
  }

  int count = 1;
  for (std::vector<std::array<int, 2>>::iterator it = currentNode->result.begin();
       it != currentNode->result.end(); ++it) {
    int r = oriRow - (*it)[0], c = oriCol - (*it)[1];
    if (r < 4 || r > 10 || c < 4 || c > 10) continue;

    if (((double)rand() / RAND_MAX) <= (1. / count)) {
      *row = r; *col = c;
      ++count;
    }
  }
}