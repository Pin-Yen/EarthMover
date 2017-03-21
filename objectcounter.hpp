#include <iostream>

#ifndef OBJECT_COUNTER_H_
#define OBJECT_COUNTER_H_

class ObjectCounter {
 public:
  static void registerNode(){ ++node; }
  static void unregisterNode() { --node; }

  static void registerVB(){ ++virtualboard; }
  static void unregisterVB() { --virtualboard; }

  static void registerPoint(){ ++point; }
  static void unregisterPoint() { --point; }

  static void printInfo() {
    std::cout << "node: " << node << std::endl
              << "virtualboard: " << virtualboard <<std::endl
              << "point: " << point << std::endl;
  }

 // private:
  static int node;
  static int virtualboard;
  static int point;
};
#endif