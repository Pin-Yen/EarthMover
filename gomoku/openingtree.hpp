#ifndef OPENING_TREE_H
#define OPENING_TREE_H

class OpeningTree {
 public:
  static void initialize();

 private:
  struct Node {
    Node *childNode[7][7];

    int score;
  };

  static void insert(int opening[7][7]);

  static Node* root;

  static bool isInitialized;
};

#endif