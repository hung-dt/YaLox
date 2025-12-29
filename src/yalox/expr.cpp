#include "expr.hpp"

#include "astprinter.hpp"
#include "interpreter.hpp"

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

LoxObject BinaryExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitBinaryExpr(*this);
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

LoxObject GroupingExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitGroupingExpr(*this);
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

LoxObject LiteralExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitLiteralExpr(*this);
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

/*---------------------------------------------------------------------------*/

LoxObject UnaryExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitUnaryExpr(*this);
}

}  // namespace lox
