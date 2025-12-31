#pragma once

#include <string>
#include <optional>
#include <variant>
#include <functional>

namespace lox {

class LoxCallable;

/*---------------------------------------------------------------------------*/

// Lox values can be number, string, bool, callable (function)
using LoxValueType = std::variant<double, std::string, bool, LoxCallable>;

// or nil
using LoxObject = std::optional<LoxValueType>;

using LoxFunction = std::function<LoxObject(const std::vector<LoxObject>&)>;

/*---------------------------------------------------------------------------*/

class LoxCallable
{
public:
  size_t arity{};

  LoxFunction call;

  std::string name{};
};

}
