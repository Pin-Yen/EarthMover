#include "memorypool.h"

#include <assert.h>
#include <iostream>

void MemoryPool::init(size_t size, int blocks) {
  // init "blocks" of "size" memory
  pool_ = new char[size * blocks]();
  // point to top of the pool
  next_ = pool_;

  // in each block, create a pointer point into next block
  //  pool_
  //    \    _______ _______ _______
  //     \  |       |       |       |
  //      --+--> ---+--> ---+--> ---+--> ...
  //     /  |_______|_______|_______|
  //    /
  //  next_
  for (int i = 0; i < blocks - 1; ++i) {
    char** ptr = reinterpret_cast<char**>(&(pool_[size * i]));
    *ptr = &pool_[size * (i + 1)];
  }
  char** ptr = reinterpret_cast<char**>(&(pool_[size * (blocks - 1)]));
  *ptr = NULL;
}

void *MemoryPool::allocate() {
  // "ptr" point to "next_"
  //  pool_
  //    \    _______ _______ _______ _______
  //     \  |///////|///////|       |       |
  //      --+-->////|///////| -> ---+--> ---+--> ...
  //        |///////|///////|/______|_______|
  //                        /
  //  ptr  ----------------/
  //  next_---------------/
  void *ptr = next_;
  assert(ptr != NULL); // Memory pool is full.

  // "next_" point to next of the "next_"
  //  pool_
  //    \    _______ _______ _______ _______
  //     \  |///////|///////|       |       |
  //      --+-->////|///////| ->    | -> ---+--> ...
  //        |///////|///////|/______|/______|
  //                        /       /
  //  ptr  ----------------/       /
  //  next_-----------------------/
  next_ = *(reinterpret_cast<char**>(&(next_[0])));

  // return the block that "ptr" point into
  return ptr;
}

void MemoryPool::deallocate(void *ptr) {
  //  pool_
  //    \    _______ _______ _______ _______
  //     \  |///////|///////|///////|       |
  //      --+-->////| ->////|///////| -> ---+--> ...
  //        |///////|/_/////|///////|/______|
  //                /               /
  //  ptr      ----/               /
  //  returnPtr---/               /
  //  next_----------------------/
  char** returnPtr = static_cast<char**>(ptr);

  // (*returnPtr) point to "next_"
  //  pool_
  //    \    _______ _______ _______ _______
  //     \  |///////|///////|///////|       |
  //      --+-->////| ->/-- |///////| -> ---+--> ...
  //        |///////|/_////\|///////|/______|
  //                /       \       /
  //  ptr      ----/         \_____/
  //  returnPtr---/               /
  //  next_----------------------/
  *returnPtr = next_;

  // "next_" point to "ptr"
  //  pool_
  //    \    _______ _______ _______ _______
  //     \  |///////|///////|///////|       |
  //      --+-->////| ->/-- |///////| -> ---+--> ...
  //        |///////|/_////\|///////|/______|
  //                /       \       /
  //  ptr      ----/         \_____/
  //  returnPtr---/
  //  next_------/
  next_ = static_cast<char*>(ptr);

  // final
  //  pool_
  //    \    _______ _______ _______ _______
  //     \  |///////|       |///////|       |
  //      --+-->////| -> -- |///////| -> ---+--> ...
  //        |///////|/_____\|///////|/______|
  //                /       \       /
  //  next_--------/         \_____/
}

void MemoryPool::free() {
  if (pool_ != NULL) {
    delete [] pool_;
    pool_ = NULL;
  }
}
