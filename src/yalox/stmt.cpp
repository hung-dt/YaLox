#include "stmt.hpp"

#include "interpreter.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

BlockStmt::BlockStmt(std::vector<StmtPtr> statements)
  : statements(std::move(statements))
{
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

void ExprStmt::execute(Interpreter& interpreter)
{
  interpreter.visitExprStmt(*this);
}

/*---------------------------------------------------------------------------*/

IfStmt::IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch)
  : condition(std::move(condition))
  , thenBranch(std::move(thenBranch))
  , elseBranch(std::move(elseBranch))
{
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

void PrintStmt::execute(Interpreter& interpreter)
{
  interpreter.visitPrintStmt(*this);
}

/*---------------------------------------------------------------------------*/

VarStmt::VarStmt(Token name, ExprPtr initializer)
  : name(std::move(name))
  , initializer(std::move(initializer))
{
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

void WhileStmt::execute(Interpreter& interpreter)
{
  interpreter.visitWhileStmt(*this);
}

}  // namespace lox
