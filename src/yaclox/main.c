#include "chunk.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------*/

/** Show a prompt and let user interact with Lox.
 */
static void repl()
{
  char line[1024];
  printf("YaClox - Yet another C Lox");
  for ( ;; ) {
    printf("> ");

    if ( !fgets(line, sizeof(line), stdin) ) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}

/*---------------------------------------------------------------------------*/

/** Read the whole source file into a newly allocated buffer and return it.
 */
static char* readFile(const char* filePath)
{
  FILE* file = fopen(filePath, "rb");
  if ( file == NULL ) {
    fprintf(stderr, "Could not open file \"%s\".\n", filePath);
    exit(ERR_IOERR);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);
  if ( buffer == NULL ) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", filePath);
    // fclose(file);
    exit(ERR_IOERR);
  }

  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if ( bytesRead < fileSize ) {
    fprintf(stderr, "Could not read file \"%s\".\n", filePath);
    // free(buffer);
    // fclose(file);
    exit(ERR_IOERR);
  }
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

/*---------------------------------------------------------------------------*/

/** Read Lox script file and run it.
 */
static void runScript(const char* scriptPath)
{
  char* source = readFile(scriptPath);
  InterpretResult result = interpret(source);
  free(source);

  if ( result == INTERPRET_COMPILE_ERROR ) exit(ERR_DATAERR);
  if ( result == INTERPRET_RUNTIME_ERROR ) exit(ERR_SOFTWARE);
}

/*---------------------------------------------------------------------------*/

int main(int argc, const char* argv[])
{
  initVM();

  if ( argc == 1 ) {
    repl();
  } else if ( argc == 2 ) {
    runScript(argv[1]);
  } else {
    fprintf(stderr, "Usage: yaclox [src_file]\n");
    exit(ERR_USAGE);
  }

  freeVM();

  return 0;
}
