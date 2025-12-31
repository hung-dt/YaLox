#pragma once

#include "types.hpp"

#include <string>

namespace lox {

/*---------------------------------------------------------------------------*/

// clang-format off
enum class TokenType
{
  // Single-character tokens
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals
  IDENTIFIER, STRING, NUMBER,

  // Keywords
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

  EoF
};
// clang-format on

/*---------------------------------------------------------------------------*/

class Token
{
public:
  Token(TokenType type, std::string lexeme, LoxObject literal, int line);

  std::string toString() const;

  TokenType type() const;
  const std::string& lexeme() const;
  const LoxObject& literal() const;
  int line() const;

private:
  const TokenType type_;
  const std::string lexeme_;
  const LoxObject literal_;
  const int line_;
};

/*---------------------------------------------------------------------------*/

std::string toString(const LoxObject&);

}

