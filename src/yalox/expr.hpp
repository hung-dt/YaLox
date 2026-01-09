#pragma once

#include "token.hpp"

#include <vector>
#include <memory>

namespace lox {

/*---------------------------------------------------------------------------*/

// Forward declare all Expr types
class Expr;
class AssignExpr;
class BinaryExpr;
class CallExpr;
class GetExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class SetExpr;
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
  virtual T visitCallExpr(CallExpr&) = 0;
  virtual T visitGetExpr(GetExpr&) = 0;
  virtual T visitGroupingExpr(GroupingExpr&) = 0;
  virtual T visitLiteralExpr(LiteralExpr&) = 0;
  virtual T visitLogicalExpr(LogicalExpr&) = 0;
  virtual T visitSetExpr(SetExpr&) = 0;
  virtual T visitUnaryExpr(UnaryExpr&) = 0;
  virtual T visitVariableExpr(VariableExpr&) = 0;
};

/*---------------------------------------------------------------------------*/

// Forward declare ExprVisitor implementations
class AstPrinter;
class Resolver;
class Interpreter;

/*---------------------------------------------------------------------------*/

/** Base class for all expression types.
 */
class Expr
{
public:
  virtual ~Expr() = default;

  // accept function for ExprVisitor<std::string>
  virtual std::string toString(AstPrinter&) = 0;

  // accept function for ExprVisitor<void>
  virtual void resolve(Resolver&) = 0;

  // accept function for ExprVisitor<LoxObject>
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

  void resolve(Resolver&) override;

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

  void resolve(Resolver&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr left;
  Token op;
  ExprPtr right;
};

/*---------------------------------------------------------------------------*/

/** Call expression.
 */
class CallExpr : public Expr
{
public:
  CallExpr(ExprPtr callee, Token closingParen, std::vector<ExprPtr> arguments);

  std::string toString(AstPrinter&) override;

  void resolve(Resolver&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr callee;
  Token closingParen;
  std::vector<ExprPtr> arguments;
};

/*---------------------------------------------------------------------------*/

/** Get expression.
 */
class GetExpr : public Expr
{
public:
  GetExpr(ExprPtr object, Token name);

  std::string toString(AstPrinter&) override;

  void resolve(Resolver&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr object;
  Token name;
};

/*---------------------------------------------------------------------------*/

/** Grouping expression.
 */
class GroupingExpr : public Expr
{
public:
  GroupingExpr(ExprPtr expression);

  std::string toString(AstPrinter&) override;

  void resolve(Resolver&) override;

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

  void resolve(Resolver&) override;

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

  void resolve(Resolver&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr left;
  Token op;
  ExprPtr right;
};

/*---------------------------------------------------------------------------*/

/** Set expression.
 */
class SetExpr : public Expr
{
public:
  SetExpr(ExprPtr object, Token name, ExprPtr value);

  std::string toString(AstPrinter&) override;

  void resolve(Resolver&) override;

  LoxObject evaluate(Interpreter&) override;

  ExprPtr object;
  Token name;
  ExprPtr value;
};

/*---------------------------------------------------------------------------*/

/** Unary expression.
 */
class UnaryExpr : public Expr
{
public:
  UnaryExpr(Token op, ExprPtr right);

  std::string toString(AstPrinter&) override;

  void resolve(Resolver&) override;

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

  void resolve(Resolver&) override;

  LoxObject evaluate(Interpreter&) override;

  Token name;
};

}  // namespace lox
