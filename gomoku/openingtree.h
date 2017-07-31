#ifndef GOMOKU_OPENINGTREE_H_
#define GOMOKU_OPENINGTREE_H_

#include <stdlib.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <vector>

template <int StatusLength>
class VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree {
 public:
  // initialize opening tree
  static void init();

  static int classify(VirtualBoardGomoku::Point point[LENGTH]);

 private:
  struct Node {
    Node() {
      for (int r = 0; r < 5; ++r)
        for (int c = 0; c < 5; ++c) {
          childNode[r][c][0] = NULL;
          childNode[r][c][1] = NULL;
        }
    }

    std::vector<std::array<int, 2>> result;

    Node *childNode[5][5][2];
  };

  // rotate "table" 90 degrees clockwise
  static void rotate(char table[5][5]);

  // mirror "table" by swapping table's rows and columns
  static void mirror(char table[5][5]);

  // insert "table" to tree
  static void insert(char table[5][5]);

  static Node* root;

 private:
  static bool isInit_;
};

#include "status.h"
#include "../virtualboard.h"
#include "virtualboardgomoku.h"
#include "point.h"
#include "evaluator.h"

template <int StatusLength>
bool VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::isInit_ = false;

template <int StatusLength>
typename VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::Node*
  VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::root = new Node();

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::init() {
  if (isInit_) return;
  isInit_ = true;

  std::ifstream file("gomoku/opening.txt");

  float openingAmout;
  while (file >> openingAmout) {
    char table[5][5];
    for (int r = 0; r < 5; ++r)
      for (int c = 0; c < 5; ++c)
        file >> table[r][c];

    // insert 8 direction
    // rotate four times -> mirror -> rotate four times
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

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::rotate(
    char table[5][5]) {
  char temp[5][5];
  // rotate 90 degrees clockwise (row -> col, col -> 4 - row)
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      temp[c][4 - r] = table[r][c];

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      table[r][c] = temp[r][c];
}

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::mirror(
    char table[5][5]) {
  char temp[5][5];
  // mirror (row -> col, col -> row)
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      temp[c][r] = table[r][c];

  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      table[r][c] = temp[r][c];
}

template <int StatusLength>
void VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::insert(
    char table[5][5]) {
  Node* currentNode = root;

  int oriR = 0, oriC = 0, curR, curC;
  // set the origin in the lower right corner in the table
  // e.g. the origin of the example below is "."
  //   X
  //   C O O C
  //     C X .
  //
  //
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      if (table[r][c] == 'X' || table[r][c] == 'O') {
        if (r > oriR)
          oriR = r;
        if (c > oriC)
          oriC = c;
      }

  // set current point to origin
  curR = oriR; curC = oriC;

  while (true) {
    // if find a occupied point
    if (table[curR][curC] == 'X' || table[curR][curC] == 'O') {
      int color = table[curR][curC] == 'X' ? 0 : 1;

      // using vector type to record (origin to current point)
      if (currentNode->childNode[oriR - curR][oriC - curC][color] == NULL)
        currentNode->childNode[oriR - curR][oriC - curC][color] = new Node();
      currentNode = currentNode->childNode[oriR - curR][oriC - curC][color];
    }

    // the insert order is from right to left, from bottom to top
    --curC;
    if (curC < 0) {
      if (curR == 0) break;
      --curR;

      curC = 4;
    }
  }

  // record the fifth move
  for (curR = 0; curR < 5; ++curR)
    for (curC = 0; curC < 5; ++curC)
      if (table[curR][curC] == 'P') {
        // using vector type to record
        std::array<int, 2> result = {oriR - curR, oriC - curC};
        // if vector didnot contain this result, add this result into vector
        if (std::find(currentNode->result.begin(),
                      currentNode->result.end(),
                      result) == currentNode->result.end())
          currentNode->result.push_back(result);
      }
}

// classify method please refer to insert
template <int StatusLength>
int VirtualBoardGomoku<StatusLength>::Evaluator::OpeningTree::classify(
    VirtualBoardGomoku<StatusLength>::Point point[LENGTH]) {
  Node* currentNode = root;

  int oriR = 0, oriC = 0;
  int left = 14, top = 14;
  // set the origin in the lower right corner, find the left and top
  // "i" is for point index
  for (int r = 0, i = 0; r < DIMEN; ++r)
    for (int c = 0; c < DIMEN; ++c, ++i)
      if (point[i].status() == BLACK || point[i].status() == WHITE) {
        if (r < top)
          top = r;
        if (c < left)
          left = c;
        if (r > oriR)
          oriR = r;
        if (c > oriC)
          oriC = c;
      }

  if (oriR - top > 4 || oriC - left > 4) return -1;

  int curR = oriR, curC = oriC;

  while (true) {
    int curIndex = curR * DIMEN + curC;
    if (point[curIndex].status() == BLACK ||
        point[curIndex].status() == WHITE) {
      STATUS color = point[curIndex].status();
      if (currentNode->childNode[oriR - curR][oriC - curC][color] == NULL)
        return -1;

      currentNode = currentNode->childNode[oriR - curR][oriC - curC][color];
    }

    --curC;
    if (curC < oriC - 4 || curC < 0) {
      if (curR == oriR - 4 || curR == 0) break;
      --curR;

      curC = oriC;
    }
  }

  int index = -1, count = 1;
  for (std::array<int, 2> result : currentNode->result) {
    int r = oriR - result[0], c = oriC - result[1];
    if (r < 4 || r > 10 || c < 4 || c > 10) continue;

    if ((static_cast<double>(rand()) / RAND_MAX) <= (1. / count)) {
      index =  r * DIMEN + c;
      ++count;
    }
  }
  return index;
}

#endif  // GOMOKU_OPENINGTREE_H_
