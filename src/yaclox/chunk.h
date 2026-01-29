#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

// Define code representation

// Each instruction has a one-byte operation code (opcode)
typedef enum
{
  OP_CONSTANT,
  OP_RETURN,
} OpCode;

// Bytecode is a series of instructions (as dynamic array)
typedef struct
{
  int capacity;
  int size;
  uint8_t* code;
  int* lines;  // source line numbers
  ValueArray constants;
} Chunk;

// Initialize a new chunk
void initChunk(Chunk* chunk);

// Append a byte to the end of the chunk
void appendChunk(Chunk* chunk, uint8_t byte, int line);

int addConstant(Chunk* chunk, Value value);

void freeChunk(Chunk* chunk);

#endif  // !clox_chunk_h
