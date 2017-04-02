#include "openingtree.hpp"

/* initialize root*/
OpeningTree::Node* OpeningTree::root = new Node();
bool OpeningTree::isInitialized = false;

void OpeningTree::initialize() {
  if (isInitialized) return;
  isInitialized = true;
}