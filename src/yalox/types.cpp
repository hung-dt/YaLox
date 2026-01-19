#include "types.hpp"
#include "interpreter.hpp"

#include <sstream>

namespace lox {

/*---------------------------------------------------------------------------*/

/** Get value of an instance's property by its name.
 */
LoxObject& LoxInstance::get(const Token& name)
{
  if ( auto it = fields.find(name.lexeme()); it != fields.end() ) {
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

LoxObject& LoxCallable::getMethod(const Token& name)
{
  if ( auto it = methods.find(name.lexeme()); it != methods.end() ) {
    return it->second;
  }

  throw RuntimeError(name, "Undefined method '" + name.lexeme() + "'.");
}

/*---------------------------------------------------------------------------*/

std::string LoxInstance::toString() const
{
  std::ostringstream os;
  os << "<" << name << ">";
  // os << "<" << name << ">:\n";
  // for ( const auto& [fieldName, fieldValue] : fields ) {
  //   os << "  " << fieldName << " = " << lox::toString(fieldValue) << '\n';
  // }
  return os.str();
}

}  // namespace lox
