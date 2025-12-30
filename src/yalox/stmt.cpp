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
  : name(std::move(name)), initializer(std::move(initializer))
{
}

/*---------------------------------------------------------------------------*/

void VarStmt::execute(Interpreter& interpreter)
{
  interpreter.visitVarStmt(*this);
}

}  // namespace lox
