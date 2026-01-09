#pragma once

#include "stmt.hpp"

#include <vector>

namespace lox {

inline const int MAX_FUNC_ARGS = 255;

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

  std::vector<StmtPtr> parse2();

private:
  const std::vector<Token>& tokens_;
  size_t current_{};

  // Grammar rules for expressions
  ExprPtr expression();
  ExprPtr assignment();
  ExprPtr logicOr();
  ExprPtr logicAnd();
  ExprPtr equality();
  ExprPtr comparison();
  ExprPtr term();
  ExprPtr factor();
  ExprPtr unary();
  ExprPtr call();
  ExprPtr finishCall(ExprPtr);
  ExprPtr primary();

  // Rules for statements
  StmtPtr declaration();
  StmtPtr classDecl();
  StmtPtr funDecl(const std::string& kind);
  StmtPtr varDecl();
  StmtPtr statement();
  StmtPtr ifStmt();
  StmtPtr printStmt();
  StmtPtr returnStmt();
  StmtPtr whileStmt();
  StmtPtr forStmt();
  StmtPtr exprStmt();
  std::vector<StmtPtr> block();

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
