#include "expr.hpp"

#include "astprinter.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

BinaryExpr::BinaryExpr(ExprPtr left, Token op, ExprPtr right)
  : left(std::move(left))
  , op(std::move(op))
  , right(std::move(right))
{
}

/*---------------------------------------------------------------------------*/

std::string BinaryExpr::toString(AstPrinter& printer)
{
  return printer.visitBinaryExpr(*this);
}

/*---------------------------------------------------------------------------*/

GroupingExpr::GroupingExpr(ExprPtr expression)
  : expression(std::move(expression))
{
}

/*---------------------------------------------------------------------------*/

std::string GroupingExpr::toString(AstPrinter& printer)
{
  return printer.visitGroupingExpr(*this);
}

/*---------------------------------------------------------------------------*/

LiteralExpr::LiteralExpr(LoxObject value)
  : value(std::move(value))
{
}

/*---------------------------------------------------------------------------*/

std::string LiteralExpr::toString(AstPrinter& printer)
{
  return printer.visitLiteralExpr(*this);
}

/*---------------------------------------------------------------------------*/

UnaryExpr::UnaryExpr(Token op, ExprPtr right)
  : op(std::move(op))
  , right(std::move(right))
{
}

/*---------------------------------------------------------------------------*/

std::string UnaryExpr::toString(AstPrinter& printer)
{
  return printer.visitUnaryExpr(*this);
}

}  // namespace lox
