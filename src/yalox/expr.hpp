#pragma once

#include "token.hpp"

#include <memory>

namespace lox {

/*---------------------------------------------------------------------------*/

// Forward declare all Expr types
class Expr;
class AssignExpr;
class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class UnaryExpr;
class VariableExpr;

using ExprPtr = std::unique_ptr<Expr>;

/*---------------------------------------------------------------------------*/

template <typename T>
class ExprVisitor
{
public:
  virtual ~ExprVisitor() = default;

  virtual T visitAssignExpr(AssignExpr&) = 0;
  virtual T visitBinaryExpr(BinaryExpr&) = 0;
  virtual T visitGroupingExpr(GroupingExpr&) = 0;
  virtual T visitLiteralExpr(LiteralExpr&) = 0;
  virtual T visitLogicalExpr(LogicalExpr&) = 0;
  virtual T visitUnaryExpr(UnaryExpr&) = 0;
  virtual T visitVariableExpr(VariableExpr&) = 0;
};

/*---------------------------------------------------------------------------*/

// Forward declare ExprVisitor implementations
class AstPrinter;
class Interpreter;

/*---------------------------------------------------------------------------*/

/** Base class for all expression types.
 */
class Expr
{
public:
  virtual ~Expr() = default;

  // accept function for ExprVisitor
  virtual std::string toString(AstPrinter&) = 0;

  // accept function for ExprVisitor
  virtual LoxObject evaluate(Interpreter&) = 0;
};

/*---------------------------------------------------------------------------*/

/** Assign expression.
 */
class AssignExpr : public Expr
{
public:
  AssignExpr(Token name, ExprPtr value);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  Token name;
  ExprPtr value;
};

/*---------------------------------------------------------------------------*/

/** Binary expression.
 */
class BinaryExpr : public Expr
{
public:
  BinaryExpr(ExprPtr left, Token op, ExprPtr right);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr left;
  Token op;
  ExprPtr right;
};

/*---------------------------------------------------------------------------*/

/** Grouping expression.
 */
class GroupingExpr : public Expr
{
public:
  GroupingExpr(ExprPtr expression);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr expression;
};

/*---------------------------------------------------------------------------*/

/** Literal expression.
 */
class LiteralExpr : public Expr
{
public:
  LiteralExpr(LoxObject value);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  LoxObject value;
};

/*---------------------------------------------------------------------------*/

/** Logical expression.
 */
class LogicalExpr : public Expr
{
public:
  LogicalExpr(ExprPtr left, Token op, ExprPtr right);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr left;
  Token op;
  ExprPtr right;
};

/*---------------------------------------------------------------------------*/

/** Unary expression.
 */
class UnaryExpr : public Expr
{
public:
  UnaryExpr(Token op, ExprPtr right);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  Token op;
  ExprPtr right;
};

/*---------------------------------------------------------------------------*/

/** Variable expression.
 */
class VariableExpr : public Expr
{
public:
  VariableExpr(Token name);

  std::string toString(AstPrinter&) override;

  LoxObject evaluate(Interpreter&) override;

  Token name;
};

}  // namespace lox
