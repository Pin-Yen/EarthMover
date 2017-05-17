#ifndef OPENING_TREE_H
#define OPENING_TREE_H

#include "stdlib.h"
#include <array>
#include <vector>

class VirtualBoardGomoku::Evaluator::OpeningTree {
 public:
  static void initialize();

  static void classify(VirtualBoardGomoku::Point* point[15][15], int *row, int *col);

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

  static void rotate(char table[5][5]);

  static void mirror(char table[5][5]);

  static void insert(char table[5][5]);

  static Node* root;
};

#endif