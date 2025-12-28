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

/** The epxression rule:
 *
 * expression -> equality ;
 */
ExprPtr Parser::expression()
{
  return equality();
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
 * primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
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

  // handle parentheses for grouping expression
  if ( match({ TokenType::LEFT_PAREN }) ) {
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExpr>(std::move(expr));
  }

  throw error(peek(), "Expect expression.");
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
