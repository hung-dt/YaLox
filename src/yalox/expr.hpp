#pragma once

#include "token.hpp"

#include <memory>

namespace lox {

/*---------------------------------------------------------------------------*/

// Forward declare all Expr types
class Expr;
class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;

using ExprPtr = std::unique_ptr<Expr>;

/*---------------------------------------------------------------------------*/

template <typename T>
class ExprVisitor
{
public:
  virtual ~ExprVisitor() = default;

  virtual T visitBinaryExpr(BinaryExpr&) = 0;
  virtual T visitGroupingExpr(GroupingExpr&) = 0;
  virtual T visitLiteralExpr(LiteralExpr&) = 0;
  virtual T visitUnaryExpr(UnaryExpr&) = 0;
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

}  // namespace lox
