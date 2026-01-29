#include "chunk.h"
#include "vm.h"

/*---------------------------------------------------------------------------*/

int main(int argc, const char* argv[])
{
  initVM();

  Chunk chunk;
  initChunk(&chunk);

  // Add a constant to the chunk's constant pool
  int constant = addConstant(&chunk, 1.2);
  appendChunk(&chunk, OP_CONSTANT, 123);

  // Write the one-byte constant index operand
  appendChunk(&chunk, (uint8_t)constant, 123);

  constant = addConstant(&chunk, 3.4);
  appendChunk(&chunk, OP_CONSTANT, 123);
  appendChunk(&chunk, (uint8_t)constant, 123);

  appendChunk(&chunk, OP_ADD, 123);

  constant = addConstant(&chunk, 5.6);
  appendChunk(&chunk, OP_CONSTANT, 123);
  appendChunk(&chunk, (uint8_t)constant, 123);

  appendChunk(&chunk, OP_DIVIDE, 123);
  appendChunk(&chunk, OP_NEGATE, 123);
  appendChunk(&chunk, OP_RETURN, 123);

  // disassembleChunk(&chunk, "test chunk");

  interpret(&chunk);

  freeVM();

  freeChunk(&chunk);

  return 0;
}
