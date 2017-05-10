#ifndef OPENING_TREE_H
#define OPENING_TREE_H

#include "stdlib.h"

class OpeningTree {
 public:
  static void initialize();

  static void classify(char table[7][7]);

 private:
  struct Node {
    Node() {
      for (int r = 0; r < 7; ++r)
        for (int c = 0; c < 7; ++c) {
          childNode[r][c][0] = NULL;
          childNode[r][c][1] = NULL;
        }
    }

    Node *childNode[7][7][2];
  };

  static void rotate(char table[7][7]);

  static void mirror(char table[7][7]);

  static void insert(char table[7][7]);

  static Node* root;
};

#endif