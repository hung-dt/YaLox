#include "scanner.h"

#include <stdbool.h>
#include <string.h>

#define CURRENT_CHAR (*scanner.current)

/*---------------------------------------------------------------------------*/

Scanner scanner;

/*---------------------------------------------------------------------------*/

static bool isDigit(const char c)
{
  return c >= '0' && c <= '9';
}

/*---------------------------------------------------------------------------*/

static bool isAlpha(const char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

/*---------------------------------------------------------------------------*/

static bool isAtEnd()
{
  return CURRENT_CHAR == '\0';
}

/*---------------------------------------------------------------------------*/

void initScanner(const char* source)
{
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}

/*---------------------------------------------------------------------------*/

static Token makeToken(TokenType type)
{
  return (Token){ .type = type,
                  .start = scanner.start,
                  .length = (int)(scanner.current - scanner.start),
                  .line = scanner.line };
}

/*---------------------------------------------------------------------------*/

static Token errorToken(const char* message)
{
  return (Token){ .type = TOKEN_ERROR,
                  .start = message,
                  .length = (int)(strlen(message)),
                  .line = scanner.line };
}

/*---------------------------------------------------------------------------*/

/** Check if current char matches with expected.
 *
 * If matches, then consume the current char and advance to next char.
 */
static bool match(const char expected)
{
  if ( isAtEnd() || CURRENT_CHAR != expected ) return false;
  scanner.current++;
  return true;
}

/*---------------------------------------------------------------------------*/

/** Skip any whitespaces and comments.
 */
static void skipWhitespaces()
{
  for ( ;; ) {
    char c = CURRENT_CHAR;
    switch ( c ) {
      case ' ':
      case '\r':
      case '\t':
        scanner.current++;
        break;
      case '\n':
        scanner.line++;
        scanner.current++;
        break;
      case '/':
        if ( match('/') ) {
          // A // comment goes until the end of the line
          while ( CURRENT_CHAR != '\n' && !isAtEnd() ) {
            scanner.current++;
          }
        } else {
          return;
        }
        break;
      default:
        return;
    }
  }
}

/*---------------------------------------------------------------------------*/

static Token scanString()
{
  while ( CURRENT_CHAR != '"' && !isAtEnd() ) {
    if ( CURRENT_CHAR == '\n' ) scanner.line++;  // support multi-line string
    scanner.current++;
  }

  if ( isAtEnd() ) return errorToken("Unterminated string.");

  // we got the closing ", so advance to next char
  scanner.current++;

  return makeToken(TOKEN_STRING);
}

/*---------------------------------------------------------------------------*/

/* Check if a given rest text is the remain of a keyword.
 *
 * @param 'offset' The offset of rest in token's lexeme
 * @param 'length' The length of rest
 * @param 'rest' The rest of the keyword
 * @param 'type' The expected type of the keyword.
 */
static TokenType
checkKeyword(int offset, int length, const char* rest, TokenType type)
{
  // The lexeme must be exactly as long as the keyword and the remaining
  // characters must match exactly.
  if (
    scanner.current - scanner.start == offset + length &&
    memcmp(scanner.start + offset, rest, length) == 0 )
    return type;

  return TOKEN_IDENTIFIER;
}

/*---------------------------------------------------------------------------*/

static TokenType identifierType()
{
  switch ( scanner.start[0] ) {
    case 'a':
      return checkKeyword(1, 2, "nd", TOKEN_AND);
    case 'c':
      return checkKeyword(1, 4, "lass", TOKEN_CLASS);
    case 'e':
      return checkKeyword(1, 3, "lse", TOKEN_ELSE);
    case 'f':
      if ( scanner.current - scanner.start > 1 ) {
        switch ( scanner.start[1] ) {
          case 'a':
            return checkKeyword(2, 3, "lse", TOKEN_FALSE);
          case 'o':
            return checkKeyword(2, 2, "or", TOKEN_FOR);
          case 'u':
            return checkKeyword(2, 1, "n", TOKEN_FUN);
        }
      }
      break;
    case 'i':
      return checkKeyword(1, 1, "f", TOKEN_IF);
    case 'n':
      return checkKeyword(1, 2, "il", TOKEN_NIL);
    case 'o':
      return checkKeyword(1, 1, "r", TOKEN_OR);
    case 'p':
      return checkKeyword(1, 4, "rint", TOKEN_PRINT);
    case 'r':
      return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
      return checkKeyword(1, 4, "uper", TOKEN_SUPER);
    case 't':
      if ( scanner.current - scanner.start > 1 ) {
        switch ( scanner.start[1] ) {
          case 'h':
            return checkKeyword(2, 2, "is", TOKEN_THIS);
          case 'r':
            return checkKeyword(2, 2, "ue", TOKEN_TRUE);
        }
      }
      break;
    case 'v':
      return checkKeyword(1, 2, "ar", TOKEN_VAR);
    case 'w':
      return checkKeyword(1, 4, "hile", TOKEN_WHILE);
  }
  return TOKEN_IDENTIFIER;
}

/*---------------------------------------------------------------------------*/

static Token scanIdentifier()
{
  while ( isAlpha(CURRENT_CHAR) || isDigit(CURRENT_CHAR) ) {
    scanner.current++;
  }

  return makeToken(identifierType());
}

/*---------------------------------------------------------------------------*/

static Token scanNumber()
{
  // consume as many digits for the integer part
  while ( isDigit(CURRENT_CHAR) ) {
    scanner.current++;
  }

  // look for fractional part
  if ( match('.') && isDigit(CURRENT_CHAR) ) {
    scanner.current++;

    while ( isDigit(CURRENT_CHAR) ) {
      scanner.current++;
    }
  }

  return makeToken(TOKEN_NUMBER);
}

/*---------------------------------------------------------------------------*/

/** Scan a single token.
 */
Token scanToken()
{
  skipWhitespaces();
  scanner.start = scanner.current;

  if ( isAtEnd() ) return makeToken(TOKEN_EOF);

  char c = CURRENT_CHAR;
  scanner.current++;

  if ( isAlpha(c) ) return scanIdentifier();
  if ( isDigit(c) ) return scanNumber();

  switch ( c ) {
    case '(':
      return makeToken(TOKEN_OPEN_PAREN);
    case ')':
      return makeToken(TOKEN_CLOSE_PAREN);
    case '{':
      return makeToken(TOKEN_OPEN_BRACE);
    case '}':
      return makeToken(TOKEN_CLOSE_BRACE);
    case ';':
      return makeToken(TOKEN_SEMICOLON);
    case ',':
      return makeToken(TOKEN_COMMA);
    case '.':
      return makeToken(TOKEN_DOT);
    case '-':
      return makeToken(TOKEN_MINUS);
    case '+':
      return makeToken(TOKEN_PLUS);
    case '/':
      return makeToken(TOKEN_SLASH);
    case '*':
      return makeToken(TOKEN_STAR);

    case '!':
      return makeToken(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return makeToken(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);

    case '"':
      return scanString();
  }
  return errorToken("Unexpected character.");
}
