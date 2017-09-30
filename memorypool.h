#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <stddef.h>

class MemoryPool {
 public:
  MemoryPool() { pool_ = NULL; }
  ~MemoryPool() { free(); }

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

#endif  // MEMORYPOOL_H_
