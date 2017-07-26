#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <iostream>

class MemoryPool {
 public:
  MemoryPool() { pool_ = NULL; };
  ~MemoryPool() { free(); };

  void init(size_t size);

  void *allocate(size_t n);

  void free();

 private:
  char *pool_, *next_;
};

#endif