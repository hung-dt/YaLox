#pragma once

#include "token.hpp"
#include "expr.hpp"

#include <vector>

namespace lox {

/*---------------------------------------------------------------------------*/

class ParserError : public std::runtime_error
{
public:
  ParserError(const std::string& message)
    : std::runtime_error{ message }
  {
  }
};

/*---------------------------------------------------------------------------*/

/** Recursive Descent Parser
 */
class Parser
{
public:
  Parser(const std::vector<Token>& tokens);

  ExprPtr parse();

private:
  const std::vector<Token>& tokens_;
  size_t current_{};

  // Grammar rules for expressions
  ExprPtr expression();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr primary();

  // Helpers
  bool match(const std::vector<TokenType>&);
  bool check(TokenType type) const;
  Token advance();
  Token peek() const;
  Token previous() const;
  Token consume(TokenType type, const std::string& message);
  bool isAtEnd() const;

  void synchronize();

  ParserError error(const Token& token, const std::string& message) const;
};
}
