#include "compiler.h"
#include "scanner.h"

#include <stdlib.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/

typedef struct
{
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;

Parser parser;
Chunk* compilingChunk;

/*---------------------------------------------------------------------------*/

static Chunk* currentChunk()
{
  return compilingChunk;
}

/*---------------------------------------------------------------------------*/

static void errorAt(Token* token, const char* message)
{
  if ( parser.panicMode ) return;
  parser.panicMode = true;

  fprintf(stderr, "[line %d] Error", token->line);

  if ( token->type == TOKEN_EOF ) {
    fprintf(stderr, " at end");
  } else if ( token->type == TOKEN_ERROR ) {
    // Nothing
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}

/*---------------------------------------------------------------------------*/

/** Report parsing error for the current token.
 */
static void errorAtCurrent(const char* message)
{
  errorAt(&parser.current, message);
}

/*---------------------------------------------------------------------------*/

static void error(const char* message)
{
  errorAt(&parser.previous, message);
}

/*---------------------------------------------------------------------------*/

static void advance()
{
  parser.previous = parser.current;

  for ( ;; ) {
    parser.current = scanToken();
    if ( parser.current.type != TOKEN_ERROR ) {
      break;
    }

    errorAtCurrent(parser.current.start);
  }
}

/*---------------------------------------------------------------------------*/

static void consume(TokenType type, const char* message)
{
  if ( parser.current.type == type ) {
    advance();
    return;
  }

  errorAtCurrent(message);
}

/*---------------------------------------------------------------------------*/

static void emitByte(uint8_t byte)
{
  appendChunk(currentChunk(), byte, parser.previous.line);
}

/*---------------------------------------------------------------------------*/

static void emitDoubleBytes(uint8_t byte1, uint8_t byte2)
{
  emitByte(byte1);
  emitByte(byte2);
}

/*---------------------------------------------------------------------------*/

static void emitReturn()
{
  emitByte(OP_RETURN);
}

/*---------------------------------------------------------------------------*/

static uint8_t makeConstant(Value value)
{
  int constantIdx = addConstant(currentChunk(), value);
  if ( constantIdx > UINT8_MAX ) {
    error("Too many constants in one chunk.");
    return 0;
  }

  return (uint8_t)constantIdx;
}

/*---------------------------------------------------------------------------*/

static void emitConstant(Value value)
{
  emitDoubleBytes(OP_CONSTANT, makeConstant(value));
}

/*---------------------------------------------------------------------------*/

static void endCompiler()
{
  emitReturn();
}

/*---------------------------------------------------------------------------*/

static void compileNumberExpr()
{
  Value value = strtod(parser.previous.start, NULL);
  emitConstant(value);
}

/*---------------------------------------------------------------------------*/

static void expression() {}

/*---------------------------------------------------------------------------*/

/** Parse unary expression.
 */
static void unary()
{
  TokenType operatorType = parser.previous.type;

  // compile the operand
  expression();

  // emit the operator instruction
  switch ( operatorType ) {
    case TOKEN_MINUS:
      emitByte(OP_NEGATE);
      break;
    default:
      return;
  }
}

/*---------------------------------------------------------------------------*/

/** Parse grouping expression.
 */
static void grouping()
{
  expression();
  consume(TOKEN_CLOSE_PAREN, "Expect ')' after expression.");
}

/*---------------------------------------------------------------------------*/

/** Compile the source code into bytecode chunk.
 *
 * @return whether the compilation succeeded or not.
 */
bool compile(const char* source, Chunk* chunk)
{
  initScanner(source);

  compilingChunk = chunk;

  parser.hadError = false;
  parser.panicMode = false;

  advance();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");

  endCompiler();

  return !parser.hadError;
}
