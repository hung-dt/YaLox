#include "token.hpp"
#include <format>

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

Token::Token(
  TokenType type,
  const std::string& lexeme,
  const LoxObject& literal,
  int line)
  : type_(type)
  , lexeme_(lexeme)
  , literal_{ literal }
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

std::string Token::toString() const
{
  std::string literal;
  if ( literal_ ) {
    if ( type_ == TokenType::NUMBER ) {
      literal = std::to_string(std::get<double>(literal_.value()));

      // if the lexeme has no dot in it, that means
      // it represents an integer literal
      if ( lexeme_.find(".") == std::string::npos ) {
        literal = lexeme_;
      }
    } else if ( type_ == TokenType::STRING ) {
      literal = std::get<std::string>(literal_.value());
    } else {
      // Boolean value
      literal = std::get<bool>(literal_.value()) ? "true" : "false";
    }
  } else {
    literal = "nil";
  }

  return std::format("{} {} {}", TokenType2String(type_), lexeme_, literal);
}

}
