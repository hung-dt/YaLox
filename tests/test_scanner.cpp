#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#undef FALSE
#undef TRUE

#include "scanner.hpp"

using namespace lox;

/*---------------------------------------------------------------------------*/

namespace {

bool isSame(const Token& tk1, const Token& tk2)
{
  return tk1.toString() == tk2.toString() and tk1.line() == tk2.line();
}

bool isSame(const std::vector<Token>& actual, const std::vector<Token> expected)
{
  if ( actual.size() != expected.size() ) return false;

  for ( auto i = 0u; i < actual.size(); ++i ) {
    if ( !isSame(actual[i], expected[i]) ) {
      std::cout << actual[i].toString() << " not match "
                << expected[i].toString() << std::endl;
      return false;
    }
  }

  return true;
}

}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner emits nil token with empty source")
{
  auto scanner = Scanner("");

  const auto tokens = scanner.scanTokens();

  CHECK(tokens.size() == 1);
  CHECK(isSame(tokens.back(), Token{ TokenType::EoF, "", std::nullopt, 1 }));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner can recognize all expected single-character and "
          "two-character tokens")
{
  auto scanner = Scanner("(){},.-+;*/!!====<<=>>=");
  const auto tokens = scanner.scanTokens();

  const auto expectedTokens = std::vector<Token>{
    Token{ TokenType::LEFT_PAREN, "(", std::nullopt, 1 },
    Token{ TokenType::RIGHT_PAREN, ")", std::nullopt, 1 },
    Token{ TokenType::LEFT_BRACE, "{", std::nullopt, 1 },
    Token{ TokenType::RIGHT_BRACE, "}", std::nullopt, 1 },
    Token{ TokenType::COMMA, ",", std::nullopt, 1 },
    Token{ TokenType::DOT, ".", std::nullopt, 1 },
    Token{ TokenType::MINUS, "-", std::nullopt, 1 },
    Token{ TokenType::PLUS, "+", std::nullopt, 1 },
    Token{ TokenType::SEMICOLON, ";", std::nullopt, 1 },
    Token{ TokenType::STAR, "*", std::nullopt, 1 },
    Token{ TokenType::SLASH, "/", std::nullopt, 1 },
    Token{ TokenType::BANG, "!", std::nullopt, 1 },
    Token{ TokenType::BANG_EQUAL, "!=", std::nullopt, 1 },
    Token{ TokenType::EQUAL_EQUAL, "==", std::nullopt, 1 },
    Token{ TokenType::EQUAL, "=", std::nullopt, 1 },
    Token{ TokenType::LESS, "<", std::nullopt, 1 },
    Token{ TokenType::LESS_EQUAL, "<=", std::nullopt, 1 },
    Token{ TokenType::GREATER, ">", std::nullopt, 1 },
    Token{ TokenType::GREATER_EQUAL, ">=", std::nullopt, 1 },
    Token{ TokenType::EoF, "", std::nullopt, 1 }
  };
  CHECK(isSame(tokens, expectedTokens));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner can recognize identifiers")
{
  auto scanner =
    Scanner("aNumber a_number number1 a_snake_case_name one2three ClassName "
            "CONST_VALUE member_ var__ _number __main__");
  const auto tokens = scanner.scanTokens();

  const auto expectedTokens = std::vector<Token>{
    Token{ TokenType::IDENTIFIER, "aNumber", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "a_number", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "number1", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "a_snake_case_name", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "one2three", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "ClassName", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "CONST_VALUE", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "member_", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "var__", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "_number", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "__main__", std::nullopt, 1 },
    Token{ TokenType::EoF, "", std::nullopt, 1 }
  };
  CHECK(isSame(tokens, expectedTokens));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner can recognize string")
{
  auto scanner = Scanner("\"this is a string\"");
  const auto tokens = scanner.scanTokens();

  const auto expectedTokens = std::vector<Token>{
    Token{ TokenType::STRING, "\"this is a string\"", "this is a string", 1 },
    Token{ TokenType::EoF, "", std::nullopt, 1 }
  };
  CHECK(isSame(tokens, expectedTokens));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner fails for unterminated string")
{
  auto scanner = Scanner("\"this is an invalid string");
  const auto tokens = scanner.scanTokens();

  const auto expectedTokens =
    std::vector<Token>{ Token{ TokenType::EoF, "", std::nullopt, 1 } };
  CHECK(isSame(tokens, expectedTokens));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner can recognize number")
{
  SUBCASE("valid numbers")
  {
    auto scanner = Scanner("123 123.45 0.001");
    const auto tokens = scanner.scanTokens();

    const auto expectedTokens =
      std::vector<Token>{ Token{ TokenType::NUMBER, "123", 123.0, 1 },
                          Token{ TokenType::NUMBER, "123.45", 123.45, 1 },
                          Token{ TokenType::NUMBER, "0.001", 0.001, 1 },
                          Token{ TokenType::EoF, "", std::nullopt, 1 } };
    CHECK(isSame(tokens, expectedTokens));
  }

  SUBCASE("number followed by an identifier or another token")
  {
    auto scanner = Scanner("12three 34.five .67");
    const auto tokens = scanner.scanTokens();

    const auto expectedTokens = std::vector<Token>{
      Token{ TokenType::NUMBER, "12", 12.0, 1 },
      Token{ TokenType::IDENTIFIER, "three", std::nullopt, 1 },
      Token{ TokenType::NUMBER, "34", 34.0, 1 },
      Token{ TokenType::DOT, ".", std::nullopt, 1 },
      Token{ TokenType::IDENTIFIER, "five", std::nullopt, 1 },
      Token{ TokenType::DOT, ".", std::nullopt, 1 },
      Token{ TokenType::NUMBER, "67", 67.0, 1 },
      Token{ TokenType::EoF, "", std::nullopt, 1 }
    };
    CHECK(isSame(tokens, expectedTokens));
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner fails for invalid characters")
{
  auto scanner = Scanner("@");
  const auto tokens = scanner.scanTokens();

  CHECK(tokens.size() == 1);
  CHECK(isSame(tokens.back(), Token{ TokenType::EoF, "", std::nullopt, 1 }));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner ignores whitespaces")
{
  auto scanner = Scanner("+ -  *\t/\t true\nfalse\n\tnil \n\nfun foo();");
  const auto tokens = scanner.scanTokens();
  const auto expectedTokens =
    std::vector<Token>{ Token{ TokenType::PLUS, "+", std::nullopt, 1 },
                        Token{ TokenType::MINUS, "-", std::nullopt, 1 },
                        Token{ TokenType::STAR, "*", std::nullopt, 1 },
                        Token{ TokenType::SLASH, "/", std::nullopt, 1 },
                        Token{ TokenType::TRUE, "true", true, 1 },
                        Token{ TokenType::FALSE, "false", false, 2 },
                        Token{ TokenType::NIL, "nil", std::nullopt, 3 },
                        Token{ TokenType::FUN, "fun", std::nullopt, 5 },
                        Token{ TokenType::IDENTIFIER, "foo", std::nullopt, 5 },
                        Token{ TokenType::LEFT_PAREN, "(", std::nullopt, 5 },
                        Token{ TokenType::RIGHT_PAREN, ")", std::nullopt, 5 },
                        Token{ TokenType::SEMICOLON, ";", std::nullopt, 5 },
                        Token{ TokenType::EoF, "", std::nullopt, 5 } };

  CHECK(isSame(tokens, expectedTokens));
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner ignores // comment ")
{
  SUBCASE("single line comment")
  {
    auto scanner = Scanner("// this is a comment");
    const auto tokens = scanner.scanTokens();

    CHECK(tokens.size() == 1);
    CHECK(isSame(tokens.back(), Token{ TokenType::EoF, "", std::nullopt, 1 }));
  }

  SUBCASE("comment out a statement")
  {
    auto scanner = Scanner("// var a = 1;");
    const auto tokens = scanner.scanTokens();

    CHECK(tokens.size() == 1);
    CHECK(isSame(tokens.back(), Token{ TokenType::EoF, "", std::nullopt, 1 }));
  }

  SUBCASE("multi-line comment")
  {
    auto scanner = Scanner("// this is a comment\n// another comment line.\n// "
                           "yet another comment line");
    const auto tokens = scanner.scanTokens();

    CHECK(tokens.size() == 1);
    CHECK(isSame(tokens.back(), Token{ TokenType::EoF, "", std::nullopt, 3 }));
  }

  SUBCASE("comment at end of line")
  {
    auto scanner = Scanner("var a = 1; // this is a comment");
    const auto tokens = scanner.scanTokens();

    const auto expectedTokens =
      std::vector<Token>{ Token{ TokenType::VAR, "var", std::nullopt, 1 },
                          Token{ TokenType::IDENTIFIER, "a", std::nullopt, 1 },
                          Token{ TokenType::EQUAL, "=", std::nullopt, 1 },
                          Token{ TokenType::NUMBER, "1", 1.0, 1 },
                          Token{ TokenType::SEMICOLON, ";", std::nullopt, 1 },
                          Token{ TokenType::EoF, "", std::nullopt, 1 } };
    CHECK(isSame(tokens, expectedTokens));
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner ignores /* */ comment") {}

/*---------------------------------------------------------------------------*/

TEST_CASE("scanner can recognize keywords")
{
  auto scanner = Scanner(
    "and andy class classic false fun func function for fore if iff nil or "
    "orchid\n"
    "print printing return super superman this true variable var while");
  const auto tokens = scanner.scanTokens();

  const auto expectedTokens = std::vector<Token>{
    Token{ TokenType::AND, "and", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "andy", std::nullopt, 1 },
    Token{ TokenType::CLASS, "class", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "classic", std::nullopt, 1 },
    Token{ TokenType::FALSE, "false", false, 1 },
    Token{ TokenType::FUN, "fun", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "func", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "function", std::nullopt, 1 },
    Token{ TokenType::FOR, "for", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "fore", std::nullopt, 1 },
    Token{ TokenType::IF, "if", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "iff", std::nullopt, 1 },
    Token{ TokenType::NIL, "nil", std::nullopt, 1 },
    Token{ TokenType::OR, "or", std::nullopt, 1 },
    Token{ TokenType::IDENTIFIER, "orchid", std::nullopt, 1 },
    Token{ TokenType::PRINT, "print", std::nullopt, 2 },
    Token{ TokenType::IDENTIFIER, "printing", std::nullopt, 2 },
    Token{ TokenType::RETURN, "return", std::nullopt, 2 },
    Token{ TokenType::SUPER, "super", std::nullopt, 2 },
    Token{ TokenType::IDENTIFIER, "superman", std::nullopt, 2 },
    Token{ TokenType::THIS, "this", std::nullopt, 2 },
    Token{ TokenType::TRUE, "true", true, 2 },
    Token{ TokenType::IDENTIFIER, "variable", std::nullopt, 2 },
    Token{ TokenType::VAR, "var", std::nullopt, 2 },
    Token{ TokenType::WHILE, "while", std::nullopt, 2 },
    Token{ TokenType::EoF, "", std::nullopt, 2 }
  };
  CHECK(isSame(tokens, expectedTokens));
}
