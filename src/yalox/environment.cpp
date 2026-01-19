#include "environment.hpp"
#include "interpreter.hpp"
#include "token.hpp"

#include <iostream>
#include <string>

namespace lox {

/*---------------------------------------------------------------------------*/

Environment::Environment(const EnvPtr& enclosing)
  : enclosing(enclosing)
{
}

/*---------------------------------------------------------------------------*/

void Environment::define(const std::string& name, LoxObject value)
{
  values_[name] = std::move(value);
}

/*---------------------------------------------------------------------------*/

const LoxObject& Environment::get(const Token& name) const
{
  // get from current scope
  if ( auto it = values_.find(name.lexeme()); it != values_.end() ) {
    return it->second;
  }

  // get from outer scope
  if ( enclosing ) {
    return enclosing->get(name);
  }

  throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
}

/*---------------------------------------------------------------------------*/

/** Get the value of the variable in the environment which defines the variable.
 *
 * It doesn't even have to check to see if the variable is there - we know it
 * will be because the resolver already found it before.
 */
const LoxObject& Environment::getAt(size_t distance, const std::string& name)
{
  return ancestor(distance).values_[name];
}

/*---------------------------------------------------------------------------*/

/** Walk a fixed number of hops up the parent chain and returns the environment
 * there.
 */
Environment& Environment::ancestor(size_t distance)
{
  auto env = this;
  for ( size_t i = 0; i < distance; ++i ) {
    env = env->enclosing.get();
  }
  return *env;
}

/*---------------------------------------------------------------------------*/

/** Assign new value to an existing variable.
 *
 * Throws if the variable does not exist.
 */
void Environment::assign(const Token& name, LoxObject value)
{
  // assign for current scope
  if ( auto it = values_.find(name.lexeme()); it != values_.end() ) {
    it->second = std::move(value);
    return;
  }

  // assign for outer scope
  if ( enclosing ) {
    enclosing->assign(name, value);
    return;
  }

  throw RuntimeError(name, "Undefined variable '" + name.lexeme() + "'.");
}

/*---------------------------------------------------------------------------*/

void Environment::assignAt(
  size_t distance,
  const Token& name,
  const LoxObject& value)
{
  ancestor(distance).values_[name.lexeme()] = value;
}

/*---------------------------------------------------------------------------*/

void Environment::print() const
{
  std::cout << "Env: " << this << '\n';
  for ( const auto& p : values_ ) {
    std::cout << "  " << p.first << ": " << toString(p.second) << '\n';
  }
  if ( enclosing ) {
    std::cout << "  outer ";
    enclosing->print();
  }
  std::cout << "----\n";
}

}
