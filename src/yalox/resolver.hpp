#pragma once

#include "stmt.hpp"

using Scope = std::unordered_map<std::string, bool>;

namespace lox {

/*---------------------------------------------------------------------------*/

class Resolver
  : public ExprVisitor<void>
  , public StmtVisitor<void>
{
public:
  Resolver(Interpreter&);

  void resolve(const std::vector<StmtPtr>&);

  void visitAssignExpr(AssignExpr&) override;
  void visitBinaryExpr(BinaryExpr&) override;
  void visitCallExpr(CallExpr&) override;
  void visitGetExpr(GetExpr&) override;
  void visitGroupingExpr(GroupingExpr&) override;
  void visitLiteralExpr(LiteralExpr&) override;
  void visitLogicalExpr(LogicalExpr&) override;
  void visitSetExpr(SetExpr&) override;
  void visitThisExpr(ThisExpr&) override;
  void visitUnaryExpr(UnaryExpr&) override;
  void visitVariableExpr(VariableExpr&) override;

  void visitBlockStmt(BlockStmt&) override;
  void visitClassStmt(ClassStmt&) override;
  void visitExprStmt(ExprStmt&) override;
  void visitFunctionStmt(FunctionStmt&) override;
  void visitIfStmt(IfStmt&) override;
  void visitPrintStmt(PrintStmt&) override;
  void visitReturnStmt(ReturnStmt&) override;
  void visitVarStmt(VarStmt&) override;
  void visitWhileStmt(WhileStmt&) override;
  void visitForStmt(ForStmt&) override;

private:
  enum class FunctionType
  {
    NONE,
    FUNC,
    METHOD
  };

  enum class ClassType
  {
    NONE,
    CLASS
  };

  Interpreter& intpr_;

  /** The scope stack is only used for local block scopes. This field keeps
   * track of the stack of scopes currently in scope. Each element in the
   * stack is a Map representing a single block scope. Keys, as in Environment,
   * are variable names. The value associated with a key in the scope map
   * represents whether or not we have finished resolving that variable's
   * initializer.
   * Variables declared at the top level in the global scope are
   * not tracked by the resolver since they are more dynamic in Lox. When
   * resolving a variable, if we can't find it in the stack of local scopes, we
   * assume it must be global.
   */
  std::vector<Scope> scopes_;

  FunctionType currentFuncType_{ FunctionType::NONE };

  ClassType currentClassType_{ ClassType::NONE };

  void resolve(Expr&);
  void resolve(Stmt&);
  void beginScope();
  void endScope();

  void declare(const Token&);
  void define(const Token&);
  void resolveLocal(Expr&, const Token&);
  void resolveFunction(FunctionStmt&, FunctionType);
};

}  // namespace lox
