#ifndef OPENING_TREE_H
#define OPENING_TREE_H

class OpeningTree {
 public:
  static void initialize();

  static void classify(int table[7][7]);

 private:
  struct Node {
    Node();

    Node *childNode[7][7][2];

    int score;
  };

  static void insert(int table[7][7]);

  static Node* root;
};

#endif