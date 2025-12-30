#pragma once

#include "expr.hpp"

#include <vector>
#include <memory>

namespace lox {

/*---------------------------------------------------------------------------*/

// Forward declare all Stmt types
class Stmt;
class BlockStmt;
class ExprStmt;
class PrintStmt;
class VarStmt;

using StmtPtr = std::unique_ptr<Stmt>;

/*---------------------------------------------------------------------------*/

template <typename T>
class StmtVisitor
{
public:
  virtual ~StmtVisitor() = default;

  virtual T visitBlockStmt(BlockStmt&) = 0;
  virtual T visitExprStmt(ExprStmt&) = 0;
  virtual T visitPrintStmt(PrintStmt&) = 0;
  virtual T visitVarStmt(VarStmt&) = 0;
};

/*---------------------------------------------------------------------------*/

/** Base class for all statement types.
 */
class Stmt
{
public:
  virtual ~Stmt() = default;

  // accept function for StmtVisitor
  virtual void execute(Interpreter&) = 0;
};

/*---------------------------------------------------------------------------*/

/** Block statement.
 */
class BlockStmt : public Stmt
{
public:
  BlockStmt(std::vector<StmtPtr> statements);

  void execute(Interpreter&) override;

  std::vector<StmtPtr> statements;
};

/*---------------------------------------------------------------------------*/

/** Expr statement.
 */
class ExprStmt : public Stmt
{
public:
  ExprStmt(ExprPtr expression);

  void execute(Interpreter&) override;

  ExprPtr expression;
};

/*---------------------------------------------------------------------------*/

/** Print statement.
 */
class PrintStmt : public Stmt
{
public:
  PrintStmt(ExprPtr expression);

  void execute(Interpreter&) override;

  ExprPtr expression;
};

/*---------------------------------------------------------------------------*/

/** Var statement.
 */
class VarStmt : public Stmt
{
public:
  VarStmt(Token name, ExprPtr initializer);

  void execute(Interpreter&) override;

  Token name;
  ExprPtr initializer;
};

}  // namespace lox
