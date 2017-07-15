#include "memorypool.hpp"

void MemoryPool::init(size_t size) {
  pool = new char[size];
  next = pool;
}

void *MemoryPool::allocate(size_t size) {
  void *ptr = next;
  next += size;
  return ptr;
}

void MemoryPool::free() {
  if (pool != NULL) {
    delete pool;
    pool = NULL;
  }
}