#include "expr.hpp"

#include "astprinter.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

BinaryExpr::BinaryExpr(ExprPtr left, const Token& op, ExprPtr right)
  : left(std::move(left))
  , op(op)
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

LiteralExpr::LiteralExpr(const LoxObject& value)
  : value(value)
{
}

/*---------------------------------------------------------------------------*/

std::string LiteralExpr::toString(AstPrinter& printer)
{
  return printer.visitLiteralExpr(*this);
}

/*---------------------------------------------------------------------------*/

UnaryExpr::UnaryExpr(const Token& op, ExprPtr right)
  : op(op)
  , right(std::move(right))
{
}

/*---------------------------------------------------------------------------*/

std::string UnaryExpr::toString(AstPrinter& printer)
{
  return printer.visitUnaryExpr(*this);
}

}  // namespace lox
