#pragma once

#include "aliases.hpp"

#include <functional>

namespace lox {

class Token;

class FunctionStmt;

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

  FunctionStmt* funcStmt{};

  LoxFunction call;

  std::string name{};

  // A class stores behavior (methods) through a map of method name to LoxObject
  // (LoxCallable) and are accessed through its instances.
  std::unordered_map<std::string, LoxObject> methods{};

  LoxObject& getMethod(const Token& name);
};

/*---------------------------------------------------------------------------*/

/** And instance of a class which holds the class definition and its fields.
 */
class LoxInstance
{
public:
  LoxCallable class_;

  std::string name{};

  // An instance stores its state (fields) through a map of field name to
  // LoxObject
  std::unordered_map<std::string, LoxObject> fields{};

  LoxObject& get(const Token& name);

  void set(const Token& name, const LoxObject& value);

  std::string toString() const;
};

}  // namespace lox
