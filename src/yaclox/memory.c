#include "memory.h"

#include <stdlib.h>

/*---------------------------------------------------------------------------*/

/* The single function used for memory management.

  | oldSize  | newSize              | Operation                   |
  |----------|----------------------|-----------------------------|
  | 0        | Non‑zero             | Allocate new block.         |
  | Non‑zero | 0                    | Free allocation.            |
  | Non‑zero | Smaller than oldSize | Shrink existing allocation. |
  | Non‑zero | Larger than oldSize  | Grow existing allocation.   |
  | cmp A, B | a                    | A > B (unsigned)            |
 */
void* reallocate(void* ptr, size_t oldSize, size_t newSize)
{
  if ( newSize == 0 ) {
    free(ptr);
    return NULL;
  }

  void* res = realloc(ptr, newSize);
  if ( res == NULL ) {
    exit(1);
  }
  return res;
}
