#pragma once

#include "countedptr.hpp"

#include <optional>
#include <string>
#include <variant>

namespace lox {

class LoxCallable;

class LoxInstance;

using LoxInstancePtr = counted_ptr<LoxInstance>;

/*---------------------------------------------------------------------------*/

// Lox values can be number, string, bool, callable (function, class)
using LoxValueType =
  std::variant<double, std::string, bool, LoxCallable, LoxInstancePtr>;

// or nil
using LoxObject = std::optional<LoxValueType>;

}
