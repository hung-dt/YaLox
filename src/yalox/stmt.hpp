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
class FunctionStmt;
class IfStmt;
class PrintStmt;
class VarStmt;
class WhileStmt;
class ForStmt;

using StmtPtr = std::unique_ptr<Stmt>;

/*---------------------------------------------------------------------------*/

template <typename T>
class StmtVisitor
{
public:
  virtual ~StmtVisitor() = default;

  virtual T visitBlockStmt(BlockStmt&) = 0;
  virtual T visitExprStmt(ExprStmt&) = 0;
  virtual T visitFunctionStmt(FunctionStmt&) = 0;
  virtual T visitIfStmt(IfStmt&) = 0;
  virtual T visitPrintStmt(PrintStmt&) = 0;
  virtual T visitVarStmt(VarStmt&) = 0;
  virtual T visitWhileStmt(WhileStmt&) = 0;
  virtual T visitForStmt(ForStmt&) = 0;
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

/** Function statement.
 */
class FunctionStmt : public Stmt
{
public:
  FunctionStmt(
    Token name,
    std::vector<Token> params,
    std::vector<StmtPtr> body);

  void execute(Interpreter&) override;

  Token name;
  std::vector<Token> params;
  std::vector<StmtPtr> body;
};

/*---------------------------------------------------------------------------*/

/** If statement.
 */
class IfStmt : public Stmt
{
public:
  IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch);

  void execute(Interpreter&) override;

  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;
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

/*---------------------------------------------------------------------------*/

/** While statement.
 */
class WhileStmt : public Stmt
{
public:
  WhileStmt(ExprPtr condition, StmtPtr body);

  void execute(Interpreter&) override;

  ExprPtr condition;
  StmtPtr body;
};

/*---------------------------------------------------------------------------*/

/** For statement.
 */
class ForStmt : public Stmt
{
public:
  ForStmt(
    StmtPtr initializer,
    ExprPtr condition,
    ExprPtr increment,
    StmtPtr body);

  void execute(Interpreter&) override;

  StmtPtr initializer;
  ExprPtr condition;
  ExprPtr increment;
  StmtPtr body;
};

}  // namespace lox
