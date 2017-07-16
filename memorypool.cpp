#include "memorypool.hpp"

#include <iostream>

void MemoryPool::init(size_t size) {
  pool = new char[size]();
  next = pool;
  std::cout << "create a memory pool contains " << size << " bytes (" << size / (1024 * 1024) << " mb)\n";
}

void *MemoryPool::allocate(size_t size) {
  void *ptr = next;
  next += size;
  return ptr;
}

void MemoryPool::free() {
  if (pool != NULL) {
    delete [] pool;
    pool = NULL;
  }
}