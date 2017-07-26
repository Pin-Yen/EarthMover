#include "memorypool.hpp"

#include <iostream>

void MemoryPool::init(size_t size, int blocks) {
  pool_ = new char[size * blocks]();
  next_ = pool_;

  for (int i = 0; i < blocks - 1; ++i) {
    char** ptr = reinterpret_cast<char**>(&(pool_[size * i]));
    *ptr = &pool_[size * (i + 1)];
  }

  std::cout << "create a memory pool contains " << size * blocks << " bytes ("
            << (size * blocks) / (1024 * 1024) << " mb)\n";
}

void *MemoryPool::allocate(size_t /*size*/) {
  void *ptr = next_;

  next_ = *(reinterpret_cast<char**>(&(next_[0])));
  return ptr;
}

void MemoryPool::deallocate(void *ptr, size_t /*size*/) {
  char** returnPtr = static_cast<char**>(ptr);
  *returnPtr = next_;
  next_ = static_cast<char*>(ptr);
}

void MemoryPool::free() {
  if (pool_ != NULL) {
    delete [] pool_;
    pool_ = NULL;
  }
}