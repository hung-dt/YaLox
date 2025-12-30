#include "environment.hpp"
#include "interpreter.hpp"
#include "token.hpp"

#include <string>

namespace lox {

/*---------------------------------------------------------------------------*/

Environment::Environment(const EnvSPtr& enclosing)
  : enclosing(enclosing)
{
}

/*---------------------------------------------------------------------------*/

void Environment::define(const std::string& name, const LoxObject& value)
{
  values_[name] = value;
}

/*---------------------------------------------------------------------------*/

LoxObject Environment::get(const Token& name) const
{
  // get from current scope
  auto it = values_.find(name.lexeme());
  if ( it != values_.end() ) {
    return it->second;
  }

  // get from outer scope
  if ( enclosing ) {
    return enclosing->get(name);
  }

  throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
}

/*---------------------------------------------------------------------------*/

/** Assign new value to an existing variable.
 *
 * Throws if the variable does not exist.
 */
void Environment::assign(const Token& name, const LoxObject& value)
{
  // assign for current scope
  auto it = values_.find(name.lexeme());
  if ( it != values_.end() ) {
    it->second = value;
    return;
  }

  // assign for outer scope
  if ( enclosing ) {
    enclosing->assign(name, value);
    return;
  }

  throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
}

}
