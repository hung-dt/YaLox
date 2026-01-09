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
  return toString(expr.value);
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitLogicalExpr(LogicalExpr& expr)
{
  return "(" + expr.op.lexeme() + " " + expr.left->toString(*this) + " " +
         expr.right->toString(*this) + ")";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitUnaryExpr(UnaryExpr& expr)
{
  return "(" + expr.op.lexeme() + " " + expr.right->toString(*this) + ")";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitVariableExpr(VariableExpr& expr)
{
  return "(var " + expr.name.lexeme() + ")";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitAssignExpr(AssignExpr& expr)
{
  return "(= " + expr.name.lexeme() + " " + expr.value->toString(*this) + ")";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitCallExpr([[maybe_unused]] CallExpr& expr)
{
  return "";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitGetExpr([[maybe_unused]] GetExpr& expr)
{
  return "";
}

/*---------------------------------------------------------------------------*/

std::string AstPrinter::visitSetExpr([[maybe_unused]] SetExpr& expr)
{
  return "";
}

}

