#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#undef FALSE
#undef TRUE

#include "parser.hpp"
#include "scanner.hpp"
#include "astprinter.hpp"

using namespace lox;

namespace {

template <typename T>
bool isExprType(Expr* expr)
{
  return expr && typeid(*expr) == typeid(T);
}

}

/*---------------------------------------------------------------------------*/

TEST_CASE("parser - primary rule")
{
  SUBCASE("is number")
  {
    auto expr = Parser{ Scanner{ "42" }.scanTokens() }.parse();

    REQUIRE(isExprType<LiteralExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "42");
  }

  SUBCASE("is string")
  {
    auto expr = Parser{ Scanner{ "\"hello world\"" }.scanTokens() }.parse();

    REQUIRE(isExprType<LiteralExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "\"hello world\"");
  }

  SUBCASE("is bool:true")
  {
    auto expr = Parser{ Scanner{ "true" }.scanTokens() }.parse();

    REQUIRE(isExprType<LiteralExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "true");
  }

  SUBCASE("is bool:false")
  {
    auto expr = Parser{ Scanner{ "false" }.scanTokens() }.parse();

    REQUIRE(isExprType<LiteralExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "false");
  }

  SUBCASE("is nil")
  {
    auto expr = Parser{ Scanner{ "nil" }.scanTokens() }.parse();

    REQUIRE(isExprType<LiteralExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "nil");
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("parser - unary rule")
{
  SUBCASE("!number")
  {
    auto expr = Parser{ Scanner{ "!42" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(! 42)");
  }

  SUBCASE("!string")
  {
    auto expr = Parser{ Scanner{ "!\"forty two\"" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(! \"forty two\")");
  }

  SUBCASE("!true")
  {
    auto expr = Parser{ Scanner{ "!true" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(! true)");
  }

  SUBCASE("!false")
  {
    auto expr = Parser{ Scanner{ "!false" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(! false)");
  }

  SUBCASE("!nil")
  {
    auto expr = Parser{ Scanner{ "!nil" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(! nil)");
  }

  SUBCASE("recursive !")
  {
    auto expr = Parser{ Scanner{ "!!true" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(! (! true))");
  }

  SUBCASE("-number")
  {
    auto expr = Parser{ Scanner{ "-12.345" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- 12.345)");
  }

  SUBCASE("-string")
  {
    auto expr = Parser{ Scanner{ "-\"forty two\"" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- \"forty two\")");
  }

  SUBCASE("-true")
  {
    auto expr = Parser{ Scanner{ "-true" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- true)");
  }

  SUBCASE("-false")
  {
    auto expr = Parser{ Scanner{ "-false" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- false)");
  }

  SUBCASE("-nil")
  {
    auto expr = Parser{ Scanner{ "-nil" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- nil)");
  }

  SUBCASE("recursive -")
  {
    auto expr = Parser{ Scanner{ "--42" }.scanTokens() }.parse();

    REQUIRE(isExprType<UnaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- (- 42))");
  }

  SUBCASE("group unary 1")
  {
    auto expr = Parser{ Scanner{ "(!false)" }.scanTokens() }.parse();

    REQUIRE(isExprType<GroupingExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(group (! false))");
  }

  SUBCASE("group unary 2")
  {
    auto expr = Parser{ Scanner{ "(-12.345)" }.scanTokens() }.parse();

    REQUIRE(isExprType<GroupingExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(group (- 12.345))");
  }

  SUBCASE("fails for +unary")
  {
    auto expr = Parser{ Scanner{ "+42" }.scanTokens() }.parse();

    REQUIRE(!isExprType<UnaryExpr>(expr.get()));
  }
}

/*---------------------------------------------------------------------------*/

TEST_CASE("parser - factor rule")
{
  SUBCASE("unary / unary 1")
  {
    auto expr = Parser{ Scanner{ "42 / 3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ 42 3)");
  }

  SUBCASE("unary / unary 2")
  {
    auto expr = Parser{ Scanner{ "-42 / 3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ (- 42) 3)");
  }

  SUBCASE("unary / unary 3")
  {
    auto expr = Parser{ Scanner{ "42 / -3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ 42 (- 3))");
  }

  SUBCASE("unary / unary 4")
  {
    auto expr = Parser{ Scanner{ "-42 / -3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ (- 42) (- 3))");
  }

  SUBCASE("unary * unary 1")
  {
    auto expr = Parser{ Scanner{ "42 * 3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* 42 3)");
  }

  SUBCASE("unary * unary 2")
  {
    auto expr = Parser{ Scanner{ "-42 * 3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* (- 42) 3)");
  }

  SUBCASE("unary * unary 3")
  {
    auto expr = Parser{ Scanner{ "42 * -3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* 42 (- 3))");
  }

  SUBCASE("unary * unary 4")
  {
    auto expr = Parser{ Scanner{ "-42 * -3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* (- 42) (- 3))");
  }

  SUBCASE("left associate 1")
  {
    auto expr = Parser{ Scanner{ "1 / 2 / 3" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ (/ 1 2) 3)");
  }

  SUBCASE("left associate 2")
  {
    auto expr = Parser{ Scanner{ "1 * 2 * 3" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* (* 1 2) 3)");
  }

  SUBCASE("left associate 3")
  {
    auto expr = Parser{ Scanner{ "1 / 2 * 3" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* (/ 1 2) 3)");
  }

  SUBCASE("left associate 4")
  {
    auto expr = Parser{ Scanner{ "1 * 2 / 3" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ (* 1 2) 3)");
  }

  SUBCASE("group takes precedence")
  {
    auto expr = Parser{ Scanner{ "1 / (2 * 3)" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(/ 1 (group (* 2 3)))");
  }
}

TEST_CASE("parser - term rule")
{
  SUBCASE("unary + unary 1")
  {
    auto expr = Parser{ Scanner{ "42 + 3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(+ 42 3)");
  }

  SUBCASE("unary + factor 1")
  {
    auto expr = Parser{ Scanner{ "42 + 3.0 * 5" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(+ 42 (* 3 5))");
  }

  SUBCASE("unary + factor 2")
  {
    auto expr = Parser{ Scanner{ "42 + 3.0 / 5" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(+ 42 (/ 3 5))");
  }

  SUBCASE("unary + factor with group")
  {
    auto expr = Parser{ Scanner{ "(42 + 3.0) * 5" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* (group (+ 42 3)) 5)");
  }

  SUBCASE("unary - unary 1")
  {
    auto expr = Parser{ Scanner{ "42 - 3.0" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- 42 3)");
  }

  SUBCASE("unary - factor 1")
  {
    auto expr = Parser{ Scanner{ "42 - 3.0 * 5" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- 42 (* 3 5))");
  }

  SUBCASE("unary - factor 2")
  {
    auto expr = Parser{ Scanner{ "42 - 3.0 / 5" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(- 42 (/ 3 5))");
  }

  SUBCASE("unary - factor with group")
  {
    auto expr = Parser{ Scanner{ "(42 - 3.0) * 5" }.scanTokens() }.parse();

    REQUIRE(isExprType<BinaryExpr>(expr.get()));
    CHECK(AstPrinter().print(*expr) == "(* (group (- 42 3)) 5)");
  }
}
