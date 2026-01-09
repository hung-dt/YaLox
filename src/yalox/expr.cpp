#include "expr.hpp"

#include "astprinter.hpp"
#include "resolver.hpp"
#include "interpreter.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

AssignExpr::AssignExpr(Token name, ExprPtr value)
  : name(std::move(name))
  , value(std::move(value))
{
}

/*---------------------------------------------------------------------------*/

std::string AssignExpr::toString(AstPrinter& printer)
{
  return printer.visitAssignExpr(*this);
}

/*---------------------------------------------------------------------------*/

void AssignExpr::resolve(Resolver& r)
{
  return r.visitAssignExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject AssignExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitAssignExpr(*this);
}

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

void BinaryExpr::resolve(Resolver& r)
{
  return r.visitBinaryExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject BinaryExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitBinaryExpr(*this);
}

/*---------------------------------------------------------------------------*/

CallExpr::CallExpr(
  ExprPtr callee,
  Token closingParen,
  std::vector<ExprPtr> arguments)
  : callee(std::move(callee))
  , closingParen(std::move(closingParen))
  , arguments(std::move(arguments))
{
}

/*---------------------------------------------------------------------------*/

std::string CallExpr::toString(AstPrinter& printer)
{
  return printer.visitCallExpr(*this);
}

/*---------------------------------------------------------------------------*/

void CallExpr::resolve(Resolver& r)
{
  return r.visitCallExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject CallExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitCallExpr(*this);
}

/*---------------------------------------------------------------------------*/

GetExpr::GetExpr(ExprPtr object, Token name)
  : object(std::move(object))
  , name(std::move(name))
{
}

/*---------------------------------------------------------------------------*/

std::string GetExpr::toString(AstPrinter& printer)
{
  return printer.visitGetExpr(*this);
}

/*---------------------------------------------------------------------------*/

void GetExpr::resolve(Resolver& r)
{
  return r.visitGetExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject GetExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitGetExpr(*this);
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

void GroupingExpr::resolve(Resolver& r)
{
  return r.visitGroupingExpr(*this);
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

void LiteralExpr::resolve(Resolver& r)
{
  return r.visitLiteralExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject LiteralExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitLiteralExpr(*this);
}

/*---------------------------------------------------------------------------*/

LogicalExpr::LogicalExpr(ExprPtr left, Token op, ExprPtr right)
  : left(std::move(left))
  , op(std::move(op))
  , right(std::move(right))
{
}

/*---------------------------------------------------------------------------*/

std::string LogicalExpr::toString(AstPrinter& printer)
{
  return printer.visitLogicalExpr(*this);
}

/*---------------------------------------------------------------------------*/

void LogicalExpr::resolve(Resolver& r)
{
  return r.visitLogicalExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject LogicalExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitLogicalExpr(*this);
}

/*---------------------------------------------------------------------------*/

SetExpr::SetExpr(ExprPtr object, Token name, ExprPtr value)
  : object(std::move(object))
  , name(std::move(name))
  , value(std::move(value))
{
}

/*---------------------------------------------------------------------------*/

std::string SetExpr::toString(AstPrinter& printer)
{
  return printer.visitSetExpr(*this);
}

/*---------------------------------------------------------------------------*/

void SetExpr::resolve(Resolver& r)
{
  return r.visitSetExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject SetExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitSetExpr(*this);
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

void UnaryExpr::resolve(Resolver& r)
{
  return r.visitUnaryExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject UnaryExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitUnaryExpr(*this);
}

/*---------------------------------------------------------------------------*/

VariableExpr::VariableExpr(Token name)
  : name(std::move(name))
{
}

/*---------------------------------------------------------------------------*/

std::string VariableExpr::toString(AstPrinter& printer)
{
  return printer.visitVariableExpr(*this);
}

/*---------------------------------------------------------------------------*/

void VariableExpr::resolve(Resolver& r)
{
  return r.visitVariableExpr(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject VariableExpr::evaluate(Interpreter& interpreter)
{
  return interpreter.visitVariableExpr(*this);
}

}  // namespace lox
