#include <iostream>

#ifndef OBJECT_COUNTER_H_
#define OBJECT_COUNTER_H_

class ObjectCounter {
 public:
  static void registerNode() { ++node; }
  static void unregisterNode() { --node; }

  static void registerVB() { ++virtualboard; }
  static void unregisterVB() { --virtualboard; }

  static void registerPoint() { ++point; }
  static void unregisterPoint() { --point; }

  static void registerCT() { ++chesstype; }
  static void unregisterCT() { --chesstype; }

  static void registerTypeTreeNode() { ++typetreeNode; }
  static void unregisterTypeTreeNode() { --typetreeNode; }

  static void printInfo() {
    std::cout << "node: " << node
              << "\nvirtualboard: " << virtualboard
              << "\npoint: " << point
              << "\nchesstype: " << chesstype
              << "\ntypetree node: " << typetreeNode << std::endl;
  }

 // private:
  static int node;
  static int virtualboard;
  static int point;
  static int chesstype;
  static int typetreeNode;
};
#endif