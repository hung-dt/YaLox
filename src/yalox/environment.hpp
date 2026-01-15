#pragma once

#include "aliases.hpp"
#include "countedptr.hpp"

#include <unordered_map>

namespace lox {

class Token;

class Environment;

using EnvPtr = counted_ptr<Environment>;

/*---------------------------------------------------------------------------*/

class Environment
{
public:
  Environment(const EnvPtr& enclosing = nullptr);

  void define(const std::string& name, LoxObject value);

  const LoxObject& get(const Token& name) const;

  const LoxObject& getAt(size_t distance, const Token& name);

  void assign(const Token& name, LoxObject value);

  void assignAt(size_t distance, const Token& name, const LoxObject& value);

  void print() const;

private:
  std::unordered_map<std::string, LoxObject> values_;
  EnvPtr enclosing;  // outer scope

  Environment& ancestor(size_t distance);
};

}
