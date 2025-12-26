#include "scanner.hpp"
#include "yalox.hpp"

#include <unordered_map>
#include <format>

using namespace lox;

namespace {

/*---------------------------------------------------------------------------*/

constexpr bool isDigit(const char c)
{
  return (c >= '0' && c <= '9');
}

/*---------------------------------------------------------------------------*/

constexpr bool isAlpha(const char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/*---------------------------------------------------------------------------*/

constexpr bool isAlphaNumeric(const char c)
{
  return isAlpha(c) || isDigit(c);
}

}

namespace lox {

/*---------------------------------------------------------------------------*/

const std::unordered_map<std::string, TokenType> KEYWORDS = {
  { "and", TokenType::AND },       { "class", TokenType::CLASS },
  { "else", TokenType::ELSE },     { "false", TokenType::FALSE },
  { "for", TokenType::FOR },       { "fun", TokenType::FUN },
  { "if", TokenType::IF },         { "nil", TokenType::NIL },
  { "or", TokenType::OR },         { "print", TokenType::PRINT },
  { "return", TokenType::RETURN }, { "super", TokenType::SUPER },
  { "this", TokenType::THIS },     { "true", TokenType::TRUE },
  { "var", TokenType::VAR },       { "while", TokenType::WHILE }
};

/*---------------------------------------------------------------------------*/

/** Constructor
 */
Scanner::Scanner(const std::string& source)
  : source_{ source }
{
}

/*---------------------------------------------------------------------------*/

std::vector<Token> Scanner::scanTokens()
{
  while ( !isAtEnd() ) {
    // we are at the beginning of the next lexeme
    start_ = current_;
    scanToken();
  }

  tokens_.emplace_back(TokenType::EoF, "", std::nullopt, line_);
  return tokens_;
}

/*---------------------------------------------------------------------------*/

/** Scan a single token.
 */
void Scanner::scanToken()
{
  // consume current_ char and advance to next char
  const auto currentChar = source_[current_++];

  switch ( currentChar ) {
    // Single-character tokens
    case '(':
      addToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      addToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      addToken(TokenType::RIGHT_BRACE);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '.':
      addToken(TokenType::DOT);
      break;
    case '-':
      addToken(TokenType::MINUS);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '*':
      addToken(TokenType::STAR);
      break;

    // One or two character tokens
    case '!':
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if ( match('/') ) {
        // Ignore comment by consuming characters until the end of the line
        while ( peek() != '\n' && !isAtEnd() ) {
          current_++;
        }
      } else {
        addToken(TokenType::SLASH);
      }
      break;

    case ' ':
    case '\r':
    case '\t':
      // ignore whitespaces
      break;

    case '\n':
      line_++;
      break;

    case '"':
      scanString();
      break;

    default:
      if ( isDigit(currentChar) ) {
        scanNumber();
      } else if ( isAlpha(currentChar) ) {
        // assume any lexeme starting with a letter or underscore is an
        // indentifier.
        scanIdentifier();
      } else {
        YaLox::error(
          line_, std::format("Unexpected character '{}'.", currentChar));
      }
      break;
  }
}

/*---------------------------------------------------------------------------*/

/** Scan for string literal and add it to token list with its value.
 */
void Scanner::scanString()
{
  while ( peek() != '"' && !isAtEnd() ) {
    if ( peek() == '\n' ) line_++;  // support multi-line string
    current_++;
  }

  if ( isAtEnd() ) {
    YaLox::error(line_, "Unterminated string.");
    return;
  }

  // we got the closing ", so advance to next char
  current_++;

  // trim the surrounding quotes
  const auto value = source_.substr(start_ + 1, current_ - start_ - 2);
  addToken(TokenType::STRING, LoxValueType{ value });
}

/*---------------------------------------------------------------------------*/

/** Scan for number literal and add to token list with its value as double.
 */
void Scanner::scanNumber()
{
  // Consume as many digits for the integer part
  while ( isDigit(peek()) ) {
    current_++;
  }

  // Look for fractional part
  if ( peek() == '.' && isDigit(peekNext()) ) {
    // consume the '.'
    current_++;

    while ( isDigit(peek()) ) {
      current_++;
    }
  }

  addToken(
    TokenType::NUMBER,
    LoxValueType{ std::stod(source_.substr(start_, current_ - start_)) });
}

/*---------------------------------------------------------------------------*/

/** Scan for identifier and add to token list.
 */
void Scanner::scanIdentifier()
{
  while ( isAlphaNumeric(peek()) ) {
    current_++;
  }

  auto text = source_.substr(start_, current_ - start_);
  auto type = TokenType::IDENTIFIER;
  auto it = KEYWORDS.find(text);
  if ( it != KEYWORDS.end() ) {
    type = it->second;
  }

  if ( type == TokenType::TRUE || type == TokenType::FALSE ) {
    addToken(type, LoxValueType{ text == "true" });
  } else {
    addToken(type);
  }
}

/*---------------------------------------------------------------------------*/

/** Check if current_ char matches with expected.
 *
 * If matches, then consume the current char and advance to next char.
 */
bool Scanner::match(char expected)
{
  if ( isAtEnd() ) return false;

  if ( source_[current_] != expected ) return false;

  current_++;
  return true;
}

/*---------------------------------------------------------------------------*/

/** Look at the current unconsumed char (one character of lookahead).
 */
char Scanner::peek() const
{
  if ( isAtEnd() ) return '\0';
  return source_[current_];
}

/*---------------------------------------------------------------------------*/

/** Two characters of lookahead
 */
char Scanner::peekNext() const
{
  if ( current_ + 1 >= source_.length() ) return '\0';
  return source_[current_ + 1];
}

/*---------------------------------------------------------------------------*/

/** Check if we have consumed all the characters.
 */
bool Scanner::isAtEnd() const
{
  return current_ >= source_.length();
}

/*---------------------------------------------------------------------------*/

/** Add a token with an optional value to the token list.
 */
void Scanner::addToken(TokenType type, const LoxObject& literal)
{
  const auto lexeme = source_.substr(start_, current_ - start_);
  tokens_.emplace_back(type, lexeme, literal, line_);
}

}
