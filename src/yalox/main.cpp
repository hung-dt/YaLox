#include "version.hpp"
#include "buildtime.hpp"

#include <iostream>

namespace {

/*---------------------------------------------------------------------------*/

void runPrompt()
{
  std::cout << YALOX_NAME << " " << YALOX_VERSION << " (" << GIT_TAG << "-"
            << GIT_COMMIT_SHA << ", " << BUILD_TIMESTAMP << ") ["
            << COMPILER_INFO << "]\n"
            << YALOX_DESCRIPTION << '\n';
  for ( ;; ) {
    std::cout << "> ";
    std::string line;
    if ( !std::getline(std::cin, line) ) {
      std::cout << "Goodbye!\n";
      break;
    }
  }
}

}

/*---------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
  if ( argc > 2 ) {
    std::cout << "Usage: yalox [script]\n";
    std::exit(64);
  } else if ( argc == 2 ) {
  } else {
    runPrompt();
  }

  return 0;
}
