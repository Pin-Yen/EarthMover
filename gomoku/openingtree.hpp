#ifndef OPENING_TREE_H
#define OPENING_TREE_H

class OpeningTree {
 public:
  static void initialize();

 private:
  struct Node {
    Node *childNode[9][9];

    int score;
  };

  /* Depth First Search */
  static void dfs();

  static Node* root;

  static bool isInitialized;
};

#endif