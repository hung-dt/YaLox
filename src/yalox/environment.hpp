#pragma once

#include "token.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace lox {

class Environment;

using EnvSPtr = std::shared_ptr<Environment>;

/*---------------------------------------------------------------------------*/

class Environment
{
public:
  Environment(const EnvSPtr& enclosing = nullptr);

  void define(const std::string& name, const LoxObject& value);

  LoxObject get(const Token& name) const;

  void assign(const Token& name, const LoxObject& value);

  EnvSPtr enclosing;  // outer scope

private:
  std::unordered_map<std::string, LoxObject> values_;
};

}
