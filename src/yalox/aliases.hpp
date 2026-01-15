#pragma once

#include <optional>
#include <string>
#include <variant>

namespace lox {

class LoxCallable;

class LoxInstance;

/*---------------------------------------------------------------------------*/

// Lox values can be number, string, bool, callable (function, class)
using LoxValueType =
  std::variant<double, std::string, bool, LoxCallable, LoxInstance>;

// or nil
using LoxObject = std::optional<LoxValueType>;

}
