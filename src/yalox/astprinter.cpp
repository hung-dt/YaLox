#include "astprinter.hpp"

#include <sstream>

namespace lox {

/*---------------------------------------------------------------------------*/

std::string AstPrinter::print(Expr& expr)
{
  return expr.toString(*this);
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitBinaryExpr(BinaryExpr& expr)
{
  return "(" + expr.op.lexeme() + " " + expr.left->toString(*this) + " " +
         expr.right->toString(*this) + ")";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitGroupingExpr(GroupingExpr& expr)
{
  return "(group " + expr.expression->toString(*this) + ")";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitLiteralExpr(LiteralExpr& expr)
{
  std::string literal{ "nil" };
  if ( expr.value ) {
    const auto& exprValue = expr.value.value();
    if ( std::holds_alternative<double>(exprValue) ) {
      // use sstream to discard zeroes from fractional part of a whole number
      std::ostringstream os;
      os << std::get<double>(exprValue);
      return os.str();
    } else if ( std::holds_alternative<bool>(exprValue) ) {
      return std::get<bool>(exprValue) ? "true" : "false";
    }
    return "\"" + std::get<std::string>(exprValue) + "\"";
  }
  return literal;
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitUnaryExpr(UnaryExpr& expr)
{
  return "(" + expr.op.lexeme() + " " + expr.right->toString(*this) + ")";
}

}
