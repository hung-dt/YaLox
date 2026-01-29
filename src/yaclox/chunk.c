#include "chunk.h"
#include "memory.h"

#include <stdlib.h>

/*---------------------------------------------------------------------------*/

/** Initialize a chunk.
 */
void initChunk(Chunk* chunk)
{
  chunk->capacity = 0;
  chunk->size = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
  initValueArray(&chunk->constants);
}

/*---------------------------------------------------------------------------*/

/** Append a byte to the end of a chunk.
 */
void appendChunk(Chunk* chunk, uint8_t byte, int line)
{
  if ( chunk->capacity < chunk->size + 1 ) {
    int oldCap = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCap);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCap, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCap, chunk->capacity);
  }

  chunk->code[chunk->size] = byte;
  chunk->lines[chunk->size] = line;
  chunk->size++;
}

/*---------------------------------------------------------------------------*/

/** Add a new constant to a given chunk.
 *
 * Return the index where the constant was appended so that we can locate that
 * same constant later.
 */
int addConstant(Chunk* chunk, Value value)
{
  appendValueArray(&chunk->constants, value);
  return chunk->constants.size - 1;
}

/*---------------------------------------------------------------------------*/

/** Free a chunk and reset its data.
 */
void freeChunk(Chunk* chunk)
{
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}
