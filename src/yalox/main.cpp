#include "yalox.hpp"

#include <iostream>

using namespace lox;

/*---------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
  if ( argc > 2 ) {
    std::cout << "Usage: yalox [script]\n";
    // EX_USAGE(64) - the command was used incorrectly
    std::exit(ERR_USAGE);
  } else if ( argc == 2 ) {
    YaLox::runScript(argv[1]);
  } else {
    YaLox::runPrompt();
  }

  return 0;
}
