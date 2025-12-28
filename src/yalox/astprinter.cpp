#include "astprinter.hpp"

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
  if ( expr.value ) {
    if ( std::holds_alternative<double>(expr.value.value()) ) {
      return std::to_string(std::get<double>(expr.value.value()));
    }
    return std::get<std::string>(expr.value.value());
  } else {
    return "nil";
  }
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitUnaryExpr(UnaryExpr& expr)
{
  return "(" + expr.op.lexeme() + " " + expr.right->toString(*this) + ")";
}

}
