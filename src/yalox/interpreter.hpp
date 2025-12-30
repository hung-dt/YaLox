#pragma once

#include "environment.hpp"
#include "stmt.hpp"

#include <vector>

namespace lox {

/*---------------------------------------------------------------------------*/

bool operator==(const LoxObject&, const LoxObject&);

/*---------------------------------------------------------------------------*/

class Interpreter
  : public ExprVisitor<LoxObject>
  , public StmtVisitor<void>
{
public:
  Interpreter();

  LoxObject interpret(Expr&);

  void interpret(const std::vector<StmtPtr>&);

  const EnvSPtr& environment() const;
  void environment(const EnvSPtr&);

  LoxObject visitAssignExpr(AssignExpr&) override;
  LoxObject visitBinaryExpr(BinaryExpr&) override;
  LoxObject visitGroupingExpr(GroupingExpr&) override;
  LoxObject visitLiteralExpr(LiteralExpr&) override;
  LoxObject visitUnaryExpr(UnaryExpr&) override;
  LoxObject visitVariableExpr(VariableExpr&) override;

  void visitBlockStmt(BlockStmt&) override;
  void visitExprStmt(ExprStmt&) override;
  void visitPrintStmt(PrintStmt&) override;
  void visitVarStmt(VarStmt&) override;

private:
  EnvSPtr env_;

  LoxObject evaluate(Expr&);

  void validateNumberOperand(const Token& op, const LoxObject& operand) const;
  void validateNumberOperands(
    const Token& op,
    const LoxObject& left,
    const LoxObject& right) const;

  bool isTruthy(const LoxObject&) const;
};

/*---------------------------------------------------------------------------*/

class RuntimeError : public std::runtime_error
{
public:
  RuntimeError(const Token&, const std::string&);

  // private:
  const Token token;
};

}
