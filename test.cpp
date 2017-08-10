#include <iostream>

using namespace std;

class Base {
 public:
  Base() {}
  ~Base() {}
  void forward() { cout << "B f\n"; act(); }
  void act() { cout << "B a\n"; }
};

class D : public Base {
 public:
  D() {}
  ~D() {}
  void forward() { cout << "D f\n"; act(); }
  void act() { cout << "D a\n"; }
};

int main() {
  Base b; b.forward();
  D d; d.forward();
  Base* dptr1 = new D(); ((D*)dptr1)->forward();
  D* dptr2 = new D(); dptr2->forward();
}