#ifndef OPENING_TREE_H
#define OPENING_TREE_H

class OpeningTree {
 public:
  static void initialize();

 private:
  struct Node {
    Node *childNode[][];

    double score;

    Node() {

    }
  };

  static Node* root;

  static bool isInitialized;
};

#endif