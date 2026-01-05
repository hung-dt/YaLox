#include "stmt.hpp"

#include "resolver.hpp"
#include "interpreter.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

BlockStmt::BlockStmt(std::vector<StmtPtr> statements)
  : statements(std::move(statements))
{
}

/*---------------------------------------------------------------------------*/

void BlockStmt::resolve(Resolver& r)
{
  return r.visitBlockStmt(*this);
}

/*---------------------------------------------------------------------------*/

void BlockStmt::execute(Interpreter& interpreter)
{
  interpreter.visitBlockStmt(*this);
}

/*---------------------------------------------------------------------------*/

ExprStmt::ExprStmt(ExprPtr expression)
  : expression(std::move(expression))
{
}

/*---------------------------------------------------------------------------*/

void ExprStmt::resolve(Resolver& r)
{
  return r.visitExprStmt(*this);
}

/*---------------------------------------------------------------------------*/

void ExprStmt::execute(Interpreter& interpreter)
{
  interpreter.visitExprStmt(*this);
}

/*---------------------------------------------------------------------------*/

FunctionStmt::FunctionStmt(
  Token name,
  std::vector<Token> params,
  std::vector<StmtPtr> body)
  : name(std::move(name))
  , params(std::move(params))
  , body(std::move(body))
{
}

/*---------------------------------------------------------------------------*/

void FunctionStmt::resolve(Resolver& r)
{
  return r.visitFunctionStmt(*this);
}

/*---------------------------------------------------------------------------*/

void FunctionStmt::execute(Interpreter& interpreter)
{
  interpreter.visitFunctionStmt(*this);
}

/*---------------------------------------------------------------------------*/

IfStmt::IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
  : condition(std::move(condition))
  , thenBranch(std::move(thenBranch))
  , elseBranch(std::move(elseBranch))
{
}

/*---------------------------------------------------------------------------*/

void IfStmt::resolve(Resolver& r)
{
  return r.visitIfStmt(*this);
}

/*---------------------------------------------------------------------------*/

void IfStmt::execute(Interpreter& interpreter)
{
  interpreter.visitIfStmt(*this);
}

/*---------------------------------------------------------------------------*/

PrintStmt::PrintStmt(ExprPtr expression)
  : expression(std::move(expression))
{
}

/*---------------------------------------------------------------------------*/

void PrintStmt::resolve(Resolver& r)
{
  return r.visitPrintStmt(*this);
}

/*---------------------------------------------------------------------------*/

void PrintStmt::execute(Interpreter& interpreter)
{
  interpreter.visitPrintStmt(*this);
}

/*---------------------------------------------------------------------------*/

ReturnStmt::ReturnStmt(Token keyword, ExprPtr value)
  : keyword(std::move(keyword))
  , value(std::move(value))
{
}

/*---------------------------------------------------------------------------*/

void ReturnStmt::resolve(Resolver& r)
{
  return r.visitReturnStmt(*this);
}

/*---------------------------------------------------------------------------*/

void ReturnStmt::execute(Interpreter& interpreter)
{
  interpreter.visitReturnStmt(*this);
}

/*---------------------------------------------------------------------------*/

VarStmt::VarStmt(Token name, ExprPtr initializer)
  : name(std::move(name))
  , initializer(std::move(initializer))
{
}

/*---------------------------------------------------------------------------*/

void VarStmt::resolve(Resolver& r)
{
  return r.visitVarStmt(*this);
}

/*---------------------------------------------------------------------------*/

void VarStmt::execute(Interpreter& interpreter)
{
  interpreter.visitVarStmt(*this);
}

/*---------------------------------------------------------------------------*/

WhileStmt::WhileStmt(ExprPtr condition, StmtPtr body)
  : condition(std::move(condition))
  , body(std::move(body))
{
}

/*---------------------------------------------------------------------------*/

void WhileStmt::resolve(Resolver& r)
{
  return r.visitWhileStmt(*this);
}

/*---------------------------------------------------------------------------*/

void WhileStmt::execute(Interpreter& interpreter)
{
  interpreter.visitWhileStmt(*this);
}

/*---------------------------------------------------------------------------*/

ForStmt::ForStmt(
  StmtPtr initializer,
  ExprPtr condition,
  ExprPtr increment,
  StmtPtr body)
  : initializer(std::move(initializer))
  , condition(std::move(condition))
  , increment(std::move(increment))
  , body(std::move(body))
{
}

/*---------------------------------------------------------------------------*/

void ForStmt::resolve(Resolver& r)
{
  return r.visitForStmt(*this);
}

/*---------------------------------------------------------------------------*/

void ForStmt::execute(Interpreter& interpreter)
{
  interpreter.visitForStmt(*this);
}

}  // namespace lox
