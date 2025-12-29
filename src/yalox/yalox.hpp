#pragma once

#include "interpreter.hpp"

#include <string>

namespace lox {

class Token;

class RuntimeError;

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
