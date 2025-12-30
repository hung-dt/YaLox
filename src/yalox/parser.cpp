#include "parser.hpp"
#include "yalox.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

/** Construct a Recursive Descent parser from a list of tokens.
 */
Parser::Parser(const std::vector<Token>& tokens)
  : tokens_(tokens)
{
}

/*---------------------------------------------------------------------------*/

/* Start parsing the token list into an AST.
 */
ExprPtr Parser::parse()
{
  try {
    return expression();
  } catch ( const ParserError& ) {
    return nullptr;
  }
}

/*---------------------------------------------------------------------------*/

/** program -> declaration* EoF ;
 */
std::vector<StmtPtr> Parser::parse2()
{
  std::vector<StmtPtr> statements{};

  while ( !isAtEnd() ) {
    statements.emplace_back(declaration());
  }

  return statements;
}

/*---------------------------------------------------------------------------*/

/** The epxression rule:
 *
 * expression -> assignment ;
 */
ExprPtr Parser::expression()
{
  return assignment();
}

/*---------------------------------------------------------------------------*/

/** assignment -> IDENTIFIER "=" assignment | logic_or ;
 */
ExprPtr Parser::assignment()
{
  // parse the left-hand side, which can be any expression of higher precedence.
  ExprPtr expr = logicOr();

  if ( match({ TokenType::EQUAL }) ) {
    auto equals = previous();
    ExprPtr value = assignment();

    // if the left-hand side expression is a valid assignment target
    auto varExpr = dynamic_cast<VariableExpr*>(expr.get());
    if ( varExpr ) {
      Token name = varExpr->name;
      return std::make_unique<AssignExpr>(std::move(name), std::move(value));
    }

    error(equals, "Invalid assignment target.");
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** logic_or -> logic_and ( "or" logic_and )* ;
 */
ExprPtr Parser::logicOr()
{
  ExprPtr expr = logicAnd();

  while ( match({ TokenType::OR }) ) {
    Token op = previous();
    ExprPtr right = logicAnd();
    expr = std::make_unique<LogicalExpr>(
      std::move(expr), std::move(op), std::move(right));
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** logic_and -> equality ( "and" equality )* ;
 */
ExprPtr Parser::logicAnd()
{
  ExprPtr expr = equality();

  while ( match({ TokenType::AND }) ) {
    Token op = previous();
    ExprPtr right = equality();
    expr = std::make_unique<LogicalExpr>(
      std::move(expr), std::move(op), std::move(right));
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** The equality rule:
 *
 * equality -> comparison ( ( "!=" | "==" ) comparison )* ;
 */
ExprPtr Parser::equality()
{
  auto expr = comparison();

  while ( match({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL }) ) {
    auto op = previous();
    auto right = comparison();
    expr = std::make_unique<BinaryExpr>(
      std::move(expr), std::move(op), std::move(right));
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** The comparison rule:
 *
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 */
ExprPtr Parser::comparison()
{
  auto expr = term();

  while ( match({ TokenType::GREATER,
                  TokenType::GREATER_EQUAL,
                  TokenType::LESS,
                  TokenType::LESS_EQUAL }) ) {
    auto op = previous();
    auto right = term();
    expr = std::make_unique<BinaryExpr>(
      std::move(expr), std::move(op), std::move(right));
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** The term rule:
 *
 * term -> factor ( ( "-" | "+" ) factor )* ;
 */
ExprPtr Parser::term()
{
  auto expr = factor();

  while ( match({ TokenType::MINUS, TokenType::PLUS }) ) {
    auto op = previous();
    auto right = factor();
    expr = std::make_unique<BinaryExpr>(
      std::move(expr), std::move(op), std::move(right));
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** The factor rule:
 *
 * factor -> unary ( ( "/" | "*" ) unary )* ;
 */
ExprPtr Parser::factor()
{
  auto expr = unary();

  while ( match({ TokenType::SLASH, TokenType::STAR }) ) {
    auto op = previous();
    auto right = unary();
    expr = std::make_unique<BinaryExpr>(
      std::move(expr), std::move(op), std::move(right));
  }

  return expr;
}

/*---------------------------------------------------------------------------*/

/** The unary rule:
 *
 * unary -> ( "!" | "-" ) unary | primary ;
 */
ExprPtr Parser::unary()
{
  if ( match({ TokenType::BANG, TokenType::MINUS }) ) {
    auto op = previous();
    auto right = unary();
    return std::make_unique<UnaryExpr>(std::move(op), std::move(right));
  }

  return primary();
}

/*---------------------------------------------------------------------------*/

/** The primary rule:
 *
 * primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" |
 * IDENTIFIER ;
 */
ExprPtr Parser::primary()
{
  if ( match({ TokenType::FALSE }) )
    return std::make_unique<LiteralExpr>(LoxValueType{ false });

  if ( match({ TokenType::TRUE }) )
    return std::make_unique<LiteralExpr>(LoxValueType{ true });

  if ( match({ TokenType::NIL }) )
    return std::make_unique<LiteralExpr>(std::nullopt);

  if ( match({ TokenType::NUMBER, TokenType::STRING }) ) {
    return std::make_unique<LiteralExpr>(previous().literal());
  }

  if ( match({ TokenType::IDENTIFIER }) ) {
    return std::make_unique<VariableExpr>(previous());
  }

  // handle parentheses for grouping expression
  if ( match({ TokenType::LEFT_PAREN }) ) {
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }

  throw error(peek(), "Expect expression.");
}

/*---------------------------------------------------------------------------*/

/** declaration -> varDecl | statement ;
 */
StmtPtr Parser::declaration()
{
  try {
    if ( match({ TokenType::VAR }) ) {
      return varDecl();
    }
    return statement();
  } catch ( const ParserError& error ) {
    synchronize();
  }
  return nullptr;
}

/*---------------------------------------------------------------------------*/

/** varDecl -> "var" IDENTIFIER ( "=" expression )? ";" ;
 */
StmtPtr Parser::varDecl()
{
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

  ExprPtr initializer{};
  if ( match({ TokenType::EQUAL }) ) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return std::make_unique<VarStmt>(std::move(name), std::move(initializer));
}

/*---------------------------------------------------------------------------*/

/** statement -> exprStmt | forStmt | ifStmt | printStmt | whileStmt | block ;
 */
StmtPtr Parser::statement()
{
  if ( match({ TokenType::FOR }) ) {
    return forStmt();
  }

  if ( match({ TokenType::IF }) ) {
    return ifStmt();
  }

  if ( match({ TokenType::PRINT }) ) {
    return printStmt();
  }

  if ( match({ TokenType::WHILE }) ) {
    return whileStmt();
  }

  if ( match({ TokenType::LEFT_BRACE }) ) {
    return block();
  }

  return exprStmt();
}

/*---------------------------------------------------------------------------*/

/** ifStmt -> "if" "(" expression ")" statement ( "else" statement )? ;
 */
StmtPtr Parser::ifStmt()
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  ExprPtr condition = expression();
  consume(TokenType::RIGHT_BRACE, "Expect ')' after if condition.");

  StmtPtr thenBranch = statement();
  StmtPtr elseBranch{};
  if ( match({ TokenType::ELSE }) ) {
    elseBranch = statement();
  }

  return std::make_unique<IfStmt>(
    std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

/*---------------------------------------------------------------------------*/

/** printStmt -> "print" expression ";"
 */
StmtPtr Parser::printStmt()
{
  auto value = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return std::make_unique<PrintStmt>(std::move(value));
}

/*---------------------------------------------------------------------------*/

/** whileStmt -> "while" "(" expression ")" statement ;
 */
StmtPtr Parser::whileStmt()
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  ExprPtr condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  StmtPtr body = statement();

  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

/*---------------------------------------------------------------------------*/

/** forStmt -> "for" "(" ( varDecl | exprStmt | ";" )
                 expression? ";"
                 expression? ")" statement ;

  Implement for stmt as a while stmt.
 */
StmtPtr Parser::forStmt()
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

  StmtPtr initializer{};
  if ( match({ TokenType::VAR }) ) {
    initializer = varDecl();
  } else {
    initializer = exprStmt();
  }

  ExprPtr condition{};
  if ( !check(TokenType::SEMICOLON) ) {
    condition = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after for loop condition.");

  ExprPtr increment{};
  if ( !check(TokenType::RIGHT_PAREN) ) {
    increment = expression();
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after 'for' clauses.");

  StmtPtr body = statement();

  // If there is an increment, it is executed after the body in each iteration
  if ( increment ) {
    std::vector<StmtPtr> block{};
    block.emplace_back(std::move(body));
    block.emplace_back(std::make_unique<ExprStmt>(std::move(increment)));
    body = std::make_unique<BlockStmt>(std::move(block));
  }

  // If the condition is omitted, we jam in true to make an infinite loop.
  if ( !condition ) {
    condition = std::make_unique<LiteralExpr>(LoxObject{ true });
  }
  body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

  // if there is an initializer, it runs once before the entire loop.
  if ( initializer ) {
    std::vector<StmtPtr> block{};
    block.emplace_back(std::move(initializer));
    block.emplace_back(std::move(body));
    body = std::make_unique<BlockStmt>(std::move(block));
  }

  return body;
}

/*---------------------------------------------------------------------------*/

/** exprStmt -> expression ";" ;
 */
StmtPtr Parser::exprStmt()
{
  auto expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_unique<ExprStmt>(std::move(expr));
}

/*---------------------------------------------------------------------------*/

/** block -> "{" declaration* "}" ;
 */
StmtPtr Parser::block()
{
  std::vector<StmtPtr> statements{};

  while ( !check(TokenType::RIGHT_BRACE) && !isAtEnd() ) {
    statements.emplace_back(declaration());
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return std::make_unique<BlockStmt>(std::move(statements));
}

/*---------------------------------------------------------------------------*/

/** Check the current token to see if it has any of the given token types.
 * If so, it consumes the token and returns true.
 * Othwerwise, it returns false and leave the current token alone.
 */
bool Parser::match(const std::vector<TokenType>& types)
{
  for ( auto type : types ) {
    if ( check(type) ) {
      advance();
      return true;
    }
  }

  return false;
}

/*---------------------------------------------------------------------------*/

/** Check if the current token is of the given type.
 */
bool Parser::check(TokenType type) const
{
  if ( isAtEnd() ) return false;
  return peek().type() == type;
}

/*---------------------------------------------------------------------------*/

/** Consume the current token and return it.
 */
Token Parser::advance()
{
  if ( !isAtEnd() ) current_++;
  return previous();
}

/*---------------------------------------------------------------------------*/

/** Get the current token.
 */
Token Parser::peek() const
{
  return tokens_[current_];
}

/*---------------------------------------------------------------------------*/

/** Get the most recently consumed token.
 */
Token Parser::previous() const
{
  return tokens_[current_ - 1];
}

/*---------------------------------------------------------------------------*/

/** Check if the current token is of the given type and consume it.
 *
 * If an unexpected token is there, then we've hit an error and throw it.
 */
Token Parser::consume(TokenType type, const std::string& message)
{
  if ( check(type) ) {
    return advance();
  }

  throw error(peek(), message);
}

/*---------------------------------------------------------------------------*/

/** Check if we reach the EoF token (ie, no more token to parse).
 */
bool Parser::isAtEnd() const
{
  return peek().type() == TokenType::EoF;
}

/*---------------------------------------------------------------------------*/

/** Synchronize on statement boundary:
 *
 * After a semicolon, we're probably finished with a statement. Most statements
 * starts with a keyword (for, if, return, while, etc.). When the next token
 * is any of those, we're probably about to start a statement.
 *
 * This method discards tokens until it thinks it has found a statement
 * boundary. After catching a ParseError, we'll call this and then we are
 * hopefully back in sync. When it workes well, we have discarded tokens that
 * would have likely caused cascaded errors anyway, and now we can parse the
 * reset of the file starting at the next statement.
 */
void Parser::synchronize()
{
  advance();

  while ( !isAtEnd() ) {
    if ( previous().type() == TokenType::SEMICOLON ) return;

    switch ( peek().type() ) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
        return;
      default:
        break;
    }

    advance();
  }
}

/*---------------------------------------------------------------------------*/

/** Report a parsing error to user.
 */
ParserError Parser::error(const Token& token, const std::string& message) const
{
  YaLox::error(token, message);
  return ParserError{ "parse error" };
}

}
