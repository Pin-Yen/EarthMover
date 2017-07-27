#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <iostream>

class MemoryPool {
 public:
  MemoryPool() { pool_ = NULL; };
  ~MemoryPool() { free(); };

  // initialize pool
  // size: each block's size
  // blocks: amounts of block
  void init(size_t size, int blocks);

  // allocate memory from pool
  void *allocate();

  // deallocate memory to pool
  void deallocate(void *ptr);

  // free all memory in pool
  void free();

 private:
  char *pool_, *next_;
};

#endif