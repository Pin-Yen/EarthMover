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

  // allocate memory from pool, size was already know in init
  void *allocate(size_t /*size*/);

  // deallocate memory to pool, size was already know in init
  void deallocate(void *ptr, size_t /*size*/);

  void free();

 private:
  char *pool_, *next_;
};

#endif