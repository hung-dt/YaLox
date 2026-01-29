#include "common.h"
#include "chunk.h"
#include "debug.h"

/*---------------------------------------------------------------------------*/

int main(int argc, const char* argv[])
{
  Chunk chunk;
  initChunk(&chunk);

  // Add a constant to the chunk's constant pool
  int constant = addConstant(&chunk, 1.2);
  appendChunk(&chunk, OP_CONSTANT, 123);

  // Write the one-byte constant index operand
  appendChunk(&chunk, (uint8_t)constant, 123);

  appendChunk(&chunk, OP_RETURN, 123);

  disassembleChunk(&chunk, "test chunk");

  freeChunk(&chunk);

  return 0;
}
