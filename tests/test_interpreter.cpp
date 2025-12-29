#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#undef FALSE
#undef TRUE

#include "interpreter.hpp"
#include "scanner.hpp"
#include "parser.hpp"

using namespace lox;

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate binary expression (op is -)")
{
  SUBCASE("number - number")
  {
    auto expr = Parser(Scanner("12 - 34").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "-22");
  }

  SUBCASE("number - string is invalid")
  {
    auto expr = Parser(Scanner("12 - \"34\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("string - number is invalid")
  {
    auto expr = Parser(Scanner("\"12\" - 34").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("string - string is invalid")
  {
    auto expr = Parser(Scanner("\"12\" - \"34\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("number - nil is invalid")
  {
    auto expr = Parser(Scanner("12 - nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("nil - number is invalid")
  {
    auto expr = Parser(Scanner("nil - 34").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("nil - nil is invalid")
  {
    auto expr = Parser(Scanner("nil - nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("number - true is invalid")
  {
    auto expr = Parser(Scanner("12 - true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("true - number is invalid")
  {
    auto expr = Parser(Scanner("true - 34").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("true - true is invalid")
  {
    auto expr = Parser(Scanner("true - true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("number - false is invalid")
  {
    auto expr = Parser(Scanner("12 - false").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("false - number is invalid")
  {
    auto expr = Parser(Scanner("false - 34").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("false - false is invalid")
  {
    auto expr = Parser(Scanner("false - false").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate binary expression (op is +)")
{
  SUBCASE("number + number")
  {
    auto expr = Parser(Scanner("12 + 34.56").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "46.56");
  }

  SUBCASE("string + string")
  {
    auto expr =
      Parser(Scanner("\"Hello\" + \", \" + \"World!\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "\"Hello, World!\"");
  }

  SUBCASE("number + string is invalid")
  {
    auto expr = Parser(Scanner("12 + \"34.56\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
  SUBCASE("string + number is invalid")
  {
    auto expr = Parser(Scanner("\"12\" + 34.56").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate binary expression (op is != and ==)")
{
  SUBCASE("nil == nil is true")
  {
    auto expr = Parser(Scanner("nil == nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }

  SUBCASE("nil == number is false")
  {
    auto expr = Parser(Scanner("nil == 42").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
  SUBCASE("nil == string is false")
  {
    auto expr = Parser(Scanner("nil == \"nil\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
  SUBCASE("nil == true is false")
  {
    auto expr = Parser(Scanner("nil == true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
  SUBCASE("nil == false is false")
  {
    auto expr = Parser(Scanner("nil == false").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }

  SUBCASE("number == nil is false")
  {
    auto expr = Parser(Scanner("42 == nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
  SUBCASE("string == nil is false")
  {
    auto expr = Parser(Scanner("\"nil\" == nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
  SUBCASE("true == nil is false")
  {
    auto expr = Parser(Scanner("true == nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
  SUBCASE("false == nil is false")
  {
    auto expr = Parser(Scanner("false == nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }

  SUBCASE("number == number 1")
  {
    auto expr = Parser(Scanner("12 == 12").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }
  SUBCASE("number == number 2")
  {
    auto expr = Parser(Scanner("12 == 12.001").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }

  SUBCASE("string == string 1")
  {
    auto expr = Parser(Scanner("\"hello\" == \"hello\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }
  SUBCASE("string == string 2")
  {
    auto expr = Parser(Scanner("\"Hello\" == \"hello\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate group expression") {}

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate literal expression")
{
  SUBCASE("number 1")
  {
    auto expr = Parser(Scanner("42").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "42");
  }

  SUBCASE("number 2")
  {
    auto expr = Parser(Scanner("3.1415926535").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "3.14159");
  }

  SUBCASE("string")
  {
    auto expr = Parser(Scanner("\"hello Lox\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "\"hello Lox\"");
  }

  SUBCASE("bool true")
  {
    auto expr = Parser(Scanner("true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }

  SUBCASE("bool false")
  {
    auto expr = Parser(Scanner("true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }

  SUBCASE("nil")
  {
    auto expr = Parser(Scanner("nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate unary expression")
{
  SUBCASE("!nil is true")
  {
    auto expr = Parser(Scanner("!nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }

  SUBCASE("!false is true")
  {
    auto expr = Parser(Scanner("!false").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "true");
  }

  SUBCASE("!true is false")
  {
    auto expr = Parser(Scanner("!true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }

  SUBCASE("!number is false")
  {
    auto expr = Parser(Scanner("!12.34").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }

  SUBCASE("!string is false")
  {
    auto expr = Parser(Scanner("!\"hello\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "false");
  }

  SUBCASE("- is not for nil")
  {
    auto expr = Parser(Scanner("-nil").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("- is not for string")
  {
    auto expr = Parser(Scanner("-\"42\"").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("- is not for bool true")
  {
    auto expr = Parser(Scanner("-true").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("- is not for bool false")
  {
    auto expr = Parser(Scanner("-false").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "nil");
  }

  SUBCASE("- is only for number")
  {
    auto expr = Parser(Scanner("-1.234").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "-1.234");
  }

  SUBCASE("- is only for numeric expression")
  {
    auto expr = Parser(Scanner("-(1+2*3)").scanTokens()).parse();
    CHECK(toString(Interpreter().interpret(*expr)) == "-7");
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("interpreter - evaluate compound expression") {}
