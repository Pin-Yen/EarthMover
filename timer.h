#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>
#include <iostream>

class Timer {
 public:
  void start() { start_ = clock(); }
  void stop() { end_ = clock(); }
  void print() {
    std::cout << "duration: "
              << static_cast<double>(end_ - start_) / CLOCKS_PER_SEC
              << std::endl;
  }

 private:
  clock_t start_, end_;
};

#endif
