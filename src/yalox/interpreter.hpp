#pragma once

#include "expr.hpp"

namespace lox {

/*---------------------------------------------------------------------------*/

bool operator==(const LoxObject&, const LoxObject&);

/*---------------------------------------------------------------------------*/

class Interpreter : public ExprVisitor<LoxObject>
{
public:
  Interpreter() = default;

  LoxObject interpret(Expr&);

  LoxObject visitBinaryExpr(BinaryExpr&) override;
  LoxObject visitGroupingExpr(GroupingExpr&) override;
  LoxObject visitLiteralExpr(LiteralExpr&) override;
  LoxObject visitUnaryExpr(UnaryExpr&) override;

private:
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
