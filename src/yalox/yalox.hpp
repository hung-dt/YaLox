#pragma once

#include "interpreter.hpp"

#include <string>

namespace lox {

class Token;

class RuntimeError;

/*---------------------------------------------------------------------------*/

// Some error codes that match with sysexits.h
inline const int ERR_USAGE = 64;     // command line usage error
inline const int ERR_DATAERR = 65;   // data format error
inline const int ERR_SOFTWARE = 70;  // internal software error

/*---------------------------------------------------------------------------*/

/** The tree-walk interpreter YaLox.
 */
class YaLox
{
public:
  static void runScript(const std::string& path);
  static void runPrompt();

  static void error(int line, const std::string& message);

  static void
  report(int line, const std::string& where, const std::string& message);

  static void error(const Token& token, const std::string& message);

  static void runtimeError(const RuntimeError&);

private:
  static Interpreter interpreter_;

  static bool hadError_;
  static bool hadRuntimeError_;

  static void run(const std::string& source);
};

}
