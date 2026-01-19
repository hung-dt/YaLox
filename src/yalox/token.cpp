#include "token.hpp"

#include <format>
#include <sstream>

namespace lox {

/*---------------------------------------------------------------------------*/

// clang-format off
constexpr std::string_view TokenType2String(TokenType type)
{
  switch ( type ) {
    case TokenType::LEFT_PAREN:       return "Type::LEFT_PAREN";
    case TokenType::RIGHT_PAREN:      return "Type::RIGHT_PAREN";
    case TokenType::LEFT_BRACE:       return "Type::LEFT_BRACE";
    case TokenType::RIGHT_BRACE:      return "Type::RIGHT_BRACE";
    case TokenType::COMMA:            return "Type::COMMA";
    case TokenType::DOT:              return "Type::DOT";
    case TokenType::MINUS:            return "Type::MINUS";
    case TokenType::PLUS:             return "Type::PLUS";
    case TokenType::SEMICOLON:        return "Type::SEMICOLON";
    case TokenType::SLASH:            return "Type::SLASH";
    case TokenType::STAR:             return "Type::STAR";
    case TokenType::BANG:             return "Type::BANG";
    case TokenType::BANG_EQUAL:       return "Type::BANG_EQUAL";
    case TokenType::EQUAL:            return "Type::EQUAL";
    case TokenType::EQUAL_EQUAL:      return "Type::EQUAL_EQUAL";
    case TokenType::GREATER:          return "Type::GREATER";
    case TokenType::GREATER_EQUAL:    return "Type::GREATER_EQUAL";
    case TokenType::LESS:             return "Type::LESS";
    case TokenType::LESS_EQUAL:       return "Type::LESS_EQUAL";
    case TokenType::IDENTIFIER:       return "Type::IDENTIFIER";
    case TokenType::STRING:           return "Type::STRING";
    case TokenType::NUMBER:           return "Type::NUMBER";
    case TokenType::CLASS:            return "Type::CLASS";
    case TokenType::ELSE:             return "Type::ELSE";
    case TokenType::FALSE:            return "Type::FALSE";
    case TokenType::FUN:              return "Type::FUN";
    case TokenType::AND:              return "Type::AND";
    case TokenType::FOR:              return "Type::FOR";
    case TokenType::IF:               return "Type::IF";
    case TokenType::NIL:              return "Type::NIL";
    case TokenType::OR:               return "Type::OR";
    case TokenType::PRINT:            return "Type::PRINT";
    case TokenType::RETURN:           return "Type::RETURN";
    case TokenType::SUPER:            return "Type::SUPER";
    case TokenType::THIS:             return "Type::THIS";
    case TokenType::TRUE:             return "Type::TRUE";
    case TokenType::VAR:              return "Type::VAR";
    case TokenType::WHILE:            return "Type::WHILE";
    case TokenType::EoF:              return "Type::EoF";
    default:                          return "UNKNOWN";
  }
}

// clang-format on

/*---------------------------------------------------------------------------*/

Token::Token(TokenType type, std::string lexeme, LoxObject literal, int line)
  : type_(type)
  , lexeme_(std::move(lexeme))
  , literal_{ std::move(literal) }
  , line_(line)
{
}

/*---------------------------------------------------------------------------*/

TokenType Token::type() const
{
  return type_;
}

/*---------------------------------------------------------------------------*/

const std::string& Token::lexeme() const
{
  return lexeme_;
}

/*---------------------------------------------------------------------------*/

const LoxObject& Token::literal() const
{
  return literal_;
}

/*---------------------------------------------------------------------------*/

int Token::line() const
{
  return line_;
}

/*---------------------------------------------------------------------------*/

/** Stringify a token.
 */
std::string Token::toString() const
{
  return std::format(
    "{} {} {}", TokenType2String(type_), lexeme_, lox::toString(literal_));
}

/*---------------------------------------------------------------------------*/

/** Stringify a LoxObject.
 */
std::string toString(const LoxObject& obj)
{
  std::string literal{ "nil" };
  if ( obj ) {
    const auto& exprValue = obj.value();
    if ( std::holds_alternative<double>(exprValue) ) {
      // use sstream to discard zeroes from fractional part of a whole number
      std::ostringstream os;
      os << std::get<double>(exprValue);
      return os.str();
    } else if ( std::holds_alternative<bool>(exprValue) ) {
      return std::get<bool>(exprValue) ? "true" : "false";
    } else if ( std::holds_alternative<LoxCallable>(exprValue) ) {
      return std::get<LoxCallable>(exprValue).name;
    } else if ( std::holds_alternative<LoxInstancePtr>(exprValue) ) {
      return std::get<LoxInstancePtr>(exprValue)->toString();
    }
    return "\"" + std::get<std::string>(exprValue) + "\"";
  }
  return literal;
}

}
