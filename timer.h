#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>
#include <iostream>

class Timer {
 public:
  void start() { start_ = clock(); }
  void stop() { end_ = clock(); times_ += (end_ - start_); }
  void clear() { times_ = 0; }
  void print() {
    std::cout << "duration: "
              << static_cast<double>(times_) / CLOCKS_PER_SEC
              << std::endl;
  }

 private:
  clock_t start_, end_;
  clock_t times_ = 0;
};

#endif  // TIMER_H_
