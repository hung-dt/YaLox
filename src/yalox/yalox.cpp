#include "version.hpp"
#include "buildtime.hpp"

#include "yalox.hpp"
#include "scanner.hpp"

#include <iostream>
#include <fstream>

namespace lox {

/*---------------------------------------------------------------------------*/

bool YaLox::hadError_ = false;

bool YaLox::hadRuntimeError_ = false;

/*---------------------------------------------------------------------------*/

/** Read Lox script file and run it.
 */
void YaLox::runScript(const std::string& filepath)
{
  std::ifstream scriptFile{ filepath };

  if ( scriptFile.is_open() ) {
    // read all contents and run the script
    auto source = std::string{ std::istreambuf_iterator<char>(scriptFile),
                               std::istreambuf_iterator<char>() };
    run(source);

    // Indicate an error in the exit code
    if ( hadError_ ) {
      // EX_DATAERR(65) - the input data was incorrect or corrupted
      std::exit(65);
    }

    if ( hadRuntimeError_ ) {
      // EX_SOFTWARE(70) - An internal software error has been detected
      std::exit(70);
    }
  } else {
    std::cerr << "Failed to open script file: " << filepath << std::endl;
  }
}

/*---------------------------------------------------------------------------*/

/** Show a prompt and let user interact with Lox.
 */
void YaLox::runPrompt()
{
  std::cout << YALOX_NAME << " " << YALOX_VERSION << " (" << GIT_TAG << "-"
            << GIT_COMMIT_SHA << ", " << BUILD_TIMESTAMP << ") ["
            << COMPILER_INFO << "]\n"
            << YALOX_DESCRIPTION << '\n';

  // REPL - Read a line of input, Evaluate it, Print the result then Loop and do
  // it over again.
  for ( ;; ) {
    std::cout << "> ";
    std::string line;
    if ( !std::getline(std::cin, line) ) {
      std::cout << "Goodbye!\n";
      break;
    }

    run(line);

    // If user makes mistake, it shouldn't kill the entire session
    hadError_ = false;
  }
}

/*---------------------------------------------------------------------------*/

/** Actually execute the source.
 */
void YaLox::run(const std::string& source)
{
  Scanner scanner{ source };
  auto tokens = scanner.scanTokens();

  for ( const auto& token : tokens ) {
    std::cout << token.toString() << std::endl;
  }
}

/*---------------------------------------------------------------------------*/

/** Tell the user some syntax error occurred on a give line.
 */
void YaLox::error(int line, const std::string& message)
{
  report(line, "", message);
}

/*---------------------------------------------------------------------------*/

/** Report the error occurred on a give line.
 */
void YaLox::report(
  int line,
  const std::string& where,
  const std::string& message)
{
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError_ = true;
}

}
