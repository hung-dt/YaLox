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

  void resolve(Expr&, size_t depth);

  LoxObject visitAssignExpr(AssignExpr&) override;
  LoxObject visitBinaryExpr(BinaryExpr&) override;
  LoxObject visitCallExpr(CallExpr&) override;
  LoxObject visitGroupingExpr(GroupingExpr&) override;
  LoxObject visitLiteralExpr(LiteralExpr&) override;
  LoxObject visitLogicalExpr(LogicalExpr&) override;
  LoxObject visitUnaryExpr(UnaryExpr&) override;
  LoxObject visitVariableExpr(VariableExpr&) override;

  void executeBlock(const std::vector<StmtPtr>&, EnvPtr&);
  void visitBlockStmt(BlockStmt&) override;
  void visitExprStmt(ExprStmt&) override;
  void visitFunctionStmt(FunctionStmt&) override;
  void visitIfStmt(IfStmt&) override;
  void visitPrintStmt(PrintStmt&) override;
  void visitReturnStmt(ReturnStmt&) override;
  void visitVarStmt(VarStmt&) override;
  void visitWhileStmt(WhileStmt&) override;
  void visitForStmt(ForStmt&) override;

  EnvPtr globals;

private:
  EnvPtr env_;

  // A map to store resolution info that associates each syntax tree node with
  // its resolved data.
  std::unordered_map<Expr*, size_t> locals_;

  LoxObject evaluate(Expr&);

  void validateNumberOperand(const Token& op, const LoxObject& operand) const;
  void validateNumberOperands(
    const Token& op,
    const LoxObject& left,
    const LoxObject& right) const;

  bool isTruthy(const LoxObject&) const;

  LoxObject lookUpVariable(const Token&, VariableExpr&);
};

/*---------------------------------------------------------------------------*/

/** An exception that emulates the return statement.
 */
class ReturnValue : public std::runtime_error
{
public:
  ReturnValue(LoxObject value);

  const LoxObject value;
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
