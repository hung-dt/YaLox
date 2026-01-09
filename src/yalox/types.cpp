#include "types.hpp"

#include "token.hpp"
#include "interpreter.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

/** Get value of an instance's property by its name.
 */
const LoxObject& LoxInstance::get(const Token& name) const
{
  const auto it = fields.find(name.lexeme());
  if ( it != fields.end() ) {
    return it->second;
  }

  // Even though methods are owned by the class, they are still accessed through
  // instances of that class.
  return class_.getMethod(name);

  throw RuntimeError(name, "Undefined property '" + name.lexeme() + "'.");
}

/*---------------------------------------------------------------------------*/

void LoxInstance::set(const Token& name, const LoxObject& value)
{
  fields[name.lexeme()] = value;
}

/*---------------------------------------------------------------------------*/

const LoxObject& LoxCallable::getMethod(const Token& name) const
{
  const auto it = methods.find(name.lexeme());
  if ( it != methods.end() ) {
    return it->second;
  }

  throw RuntimeError(name, "Undefined method '" + name.lexeme() + "'.");
}

}  // namespace lox
