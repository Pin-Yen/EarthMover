#include "memorypool.hpp"

#include <iostream>

void MemoryPool::init(size_t size) {
  pool_ = new char[size]();
  next_ = pool_;
  std::cout << "create a memory pool contains " << size << " bytes (" << size / (1024 * 1024) << " mb)\n";
}

void *MemoryPool::allocate(size_t size) {
  void *ptr = next_;
  next_ += size;
  return ptr;
}

void MemoryPool::free() {
  if (pool_ != NULL) {
    delete [] pool_;
    pool_ = NULL;
  }
}