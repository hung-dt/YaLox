#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

#define GROW_CAPACITY(cap) ((cap) < 8 ? 8 : (cap) * 2)

#define GROW_ARRAY(type, ptr, oldSize, newSize)                              \
  (type*)reallocate(ptr, sizeof(type) * (oldSize), sizeof(type) * (newSize))

#define FREE_ARRAY(type, ptr, oldSize)         \
  reallocate(ptr, sizeof(type) * (oldSize), 0)

// The single function used for all dynamic memory management: allocating,
// freeing and changing the size of an existing allocation.
void* reallocate(void* ptr, size_t oldSize, size_t newSize);

#endif  // !clox_memory_h
