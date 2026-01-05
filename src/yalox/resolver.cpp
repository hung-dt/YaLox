#include "resolver.hpp"
#include "yalox.hpp"
#include <cassert>

namespace lox {

/*---------------------------------------------------------------------------*/

Resolver::Resolver(Interpreter& intpr)
  : intpr_(intpr)
{
}

/*---------------------------------------------------------------------------*/

void Resolver::visitAssignExpr(AssignExpr& expr)
{
  resolve(*(expr.value));
  resolveLocal(expr, expr.name);
}

/*---------------------------------------------------------------------------*/

void Resolver::visitBinaryExpr(BinaryExpr& expr)
{
  resolve(*(expr.left));
  resolve(*(expr.right));
}

/*---------------------------------------------------------------------------*/

void Resolver::visitCallExpr(CallExpr& expr)
{
  resolve(*(expr.callee));

  for ( const auto& arg : expr.arguments ) {
    resolve(*arg);
  }
}

/*---------------------------------------------------------------------------*/

void Resolver::visitGroupingExpr(GroupingExpr& expr)
{
  resolve(*(expr.expression));
}

/*---------------------------------------------------------------------------*/

void Resolver::visitLiteralExpr(LiteralExpr& /* unused */) {}

/*---------------------------------------------------------------------------*/

void Resolver::visitLogicalExpr(LogicalExpr& expr)
{
  resolve(*(expr.left));
  resolve(*(expr.right));
}

/*---------------------------------------------------------------------------*/

void Resolver::visitUnaryExpr(UnaryExpr& expr)
{
  resolve(*(expr.right));
}

/*---------------------------------------------------------------------------*/

void Resolver::visitVariableExpr(VariableExpr& expr)
{
  // Check to see if the variable is being accessed inside its own initliazer.
  if ( !scopes_.empty() ) {
    // If the variable exists in the current scope but its value is false, that
    // means we have declared it but not yet defined it. We report that error.
    const auto it = scopes_.back().find(expr.name.lexeme());
    if ( it != scopes_.back().end() && it->second == false ) {
      YaLox::error(
        expr.name, "Cannot read local variable in its own initializer.");
    }
  }

  resolveLocal(expr, expr.name);
}

/*---------------------------------------------------------------------------*/

void Resolver::visitBlockStmt(BlockStmt& stmt)
{
  beginScope();
  resolve(stmt.statements);
  endScope();
}

/*---------------------------------------------------------------------------*/

void Resolver::visitExprStmt(ExprStmt& stmt)
{
  resolve(*(stmt.expression));
}

/*---------------------------------------------------------------------------*/

/** Resolve function declaration.
 */
void Resolver::visitFunctionStmt(FunctionStmt& stmt)
{
  // We declare and define the name of the function in the current scope. Unlike
  // variables, though, we define the name eagerly, before resolving the
  // function’s body. This lets a function recursively refer to itself inside
  // its own body.
  declare(stmt.name);
  define(stmt.name);

  resolveFunction(stmt, FunctionType::FUNC);
}

/*---------------------------------------------------------------------------*/

void Resolver::visitIfStmt(IfStmt& stmt)
{
  resolve(*(stmt.condition));
  resolve(*(stmt.thenBranch));
  if ( stmt.elseBranch ) {
    resolve(*(stmt.elseBranch));
  }
}

/*---------------------------------------------------------------------------*/

void Resolver::visitPrintStmt(PrintStmt& stmt)
{
  resolve(*(stmt.expression));
}

/*---------------------------------------------------------------------------*/

void Resolver::visitReturnStmt(ReturnStmt& stmt)
{
  if ( currentFuncType_ == FunctionType::NONE ) {
    YaLox::error(stmt.keyword, "Cannot return from top-level code.");
  }

  if ( stmt.value ) {
    resolve(*(stmt.value));
  }
}

/*---------------------------------------------------------------------------*/

void Resolver::visitVarStmt(VarStmt& stmt)
{
  declare(stmt.name);
  if ( stmt.initializer ) {
    resolve(*(stmt.initializer));
  }
  define(stmt.name);
}

/*---------------------------------------------------------------------------*/

void Resolver::visitWhileStmt(WhileStmt& stmt)
{
  resolve(*(stmt.condition));
  resolve(*(stmt.body));
}

/*---------------------------------------------------------------------------*/

void Resolver::visitForStmt(ForStmt& stmt)
{
  if ( stmt.initializer ) {
    resolve(*(stmt.initializer));
  }

  if ( stmt.condition ) {
    resolve(*(stmt.condition));
  }

  assert(stmt.body);
  resolve(*(stmt.body));

  if ( stmt.increment ) {
    resolve(*(stmt.increment));
  }
}

/*---------------------------------------------------------------------------*/

void Resolver::resolve(const std::vector<StmtPtr>& block)
{
  for ( auto& stmt : block ) {
    resolve(*stmt);
  }
}

/*---------------------------------------------------------------------------*/

void Resolver::resolve(Stmt& stmt)
{
  stmt.resolve(*this);
}

/*---------------------------------------------------------------------------*/

void Resolver::resolve(Expr& expr)
{
  expr.resolve(*this);
}

/*---------------------------------------------------------------------------*/

/** Create a new block scope on the scopes stack.
 */
void Resolver::beginScope()
{
  scopes_.emplace_back(Scope{});
}

/*---------------------------------------------------------------------------*/

void Resolver::endScope()
{
  scopes_.pop_back();
}

/*---------------------------------------------------------------------------*/

/** Declaration adds the variable to the innermost scope so that it shadows any
 * outer one and so that we know the variable exists. We mark it as "not ready
 * yet" (false), that means we have not finished resolving that variable's
 * initializer.
 */
void Resolver::declare(const Token& name)
{
  if ( scopes_.empty() ) return;

  if ( scopes_.back().contains(name.lexeme()) ) {
    YaLox::error(name, "Already a variable with this name in this scope.");
  }

  scopes_.back()[name.lexeme()] = false;
}

/*---------------------------------------------------------------------------*/

/** After declaring the variable, we resolve its initializer expression in that
 * same scope where the new variable now exists but is unavailable. Once the
 * initializer expression is done, the variable is ready for prime time. We do
 * that by defining it.
 */
void Resolver::define(const Token& name)
{
  if ( scopes_.empty() ) return;
  scopes_.back()[name.lexeme()] = true;
}

/*---------------------------------------------------------------------------*/

/** Each time the resolver visits a variable, it tells the interpreter how many
 * scopes there are between the current scope and the scope where the variable
 * is defined.
 */
void Resolver::resolveLocal(Expr& expr, const Token& name)
{
  if ( scopes_.empty() ) return;

  for ( int i = scopes_.size() - 1; i >= 0; --i ) {
    if ( scopes_[i].contains(name.lexeme()) ) {
      intpr_.resolve(expr, scopes_.size() - 1 - i);
      return;
    }
  }
}

/*---------------------------------------------------------------------------*/

/** Resolve function's body.
 *
 * It creates a new scope for the body and then binds variables for each of the
 * function's parameters.
 *
 * Once that's ready, it resolves the function body in that scope. This is
 * different from how the interpreter handles function declarations. At runtime,
 * declaring a function doesn’t do anything with the function’s body. The body
 * doesn’t get touched until later when the function is called. In a static
 * analysis, we immediately traverse into the body right then and there.
 */
void Resolver::resolveFunction(FunctionStmt& func, FunctionType type)
{
  auto enclosingFuncType = currentFuncType_;
  currentFuncType_ = type;

  beginScope();
  for ( const auto& param : func.params ) {
    declare(param);
    define(param);
  }
  resolve(func.body);
  endScope();

  currentFuncType_ = enclosingFuncType;
}

}  // namespace lox
