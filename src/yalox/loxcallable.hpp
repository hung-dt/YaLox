#pragma once

#include "aliases.hpp"
#include "environment.hpp"

#include <functional>

namespace lox {

/*---------------------------------------------------------------------------*/

using LoxFunction = std::function<LoxObject(const std::vector<LoxObject>&)>;

/*---------------------------------------------------------------------------*/

/** Lox callable object can be a function or a class. If it is a function,
 * then a function call will invoke the call member. If it is a class, the call
 * member is just a function that returns class name.
 */
class LoxCallable
{
public:
  size_t arity{};

  LoxFunction call;

  LoxObject operator()(const std::vector<LoxObject>& args);

  std::string name{};

  // A class stores behavior (methods) through a map of method name to LoxObject
  // (LoxCallable) and are accessed through its instances.
  std::unordered_map<std::string, LoxObject> methods{};

  EnvPtr closure{};

  LoxObject& getMethod(const Token& name);
};

}
