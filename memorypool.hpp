#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

class MemoryPool {
 public:
  void *allocate(size_t n);

  void deallocate(void *ptr, size_t n);
};

#endif