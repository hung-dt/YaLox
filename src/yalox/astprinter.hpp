#pragma once

#include "expr.hpp"

#include <string>

namespace lox {

/*---------------------------------------------------------------------------*/

class AstPrinter : public ExprVisitor<std::string>
{
public:
  std::string print(Expr&);

  virtual std::string visitLiteralExpr(LiteralExpr&) override;
  virtual std::string visitLogicalExpr(LogicalExpr&) override;
  virtual std::string visitGroupingExpr(GroupingExpr&) override;
  virtual std::string visitUnaryExpr(UnaryExpr&) override;
  virtual std::string visitBinaryExpr(BinaryExpr&) override;
  virtual std::string visitVariableExpr(VariableExpr&) override;
  virtual std::string visitAssignExpr(AssignExpr&) override;
};

}
