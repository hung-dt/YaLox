#pragma once

#include "token.hpp"
#include "countedptr.hpp"

#include <string>
#include <unordered_map>

namespace lox {

class Environment;

using EnvPtr = counted_ptr<Environment>;

/*---------------------------------------------------------------------------*/

class Environment
{
public:
  Environment(const EnvPtr& enclosing = nullptr);

  void define(const std::string& name, const LoxObject& value);

  LoxObject get(const Token& name) const;

  void assign(const Token& name, const LoxObject& value);

  EnvPtr enclosing;  // outer scope

  void print() const;

private:
  std::unordered_map<std::string, LoxObject> values_;
};

}
