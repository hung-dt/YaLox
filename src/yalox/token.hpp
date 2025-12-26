#pragma once

#include <optional>
#include <string>
#include <variant>

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

// Lox values can be number, string, bool
using LoxValueType = std::variant<double, std::string, bool>;

// or nil
using LoxObject = std::optional<LoxValueType>;

/*---------------------------------------------------------------------------*/

class Token
{
public:
  Token(
    TokenType type,
    const std::string& lexeme,
    const LoxObject& literal,
    int line);

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

}

