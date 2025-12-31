#pragma once

#include "token.hpp"

#include <string>
#include <unordered_map>

namespace lox {

class Environment;

/*---------------------------------------------------------------------------*/

class Environment
{
public:
  Environment(Environment* enclosing = nullptr);

  void define(const std::string& name, const LoxObject& value);

  LoxObject get(const Token& name) const;

  void assign(const Token& name, const LoxObject& value);

  Environment* enclosing;  // outer scope

private:
  std::unordered_map<std::string, LoxObject> values_;
};

}
