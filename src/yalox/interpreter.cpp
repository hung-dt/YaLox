#include "interpreter.hpp"
#include "yalox.hpp"

#include <iostream>

namespace lox {

/*---------------------------------------------------------------------------*/

/** Execute a block statement in the context of a given environment.
 */
class BlockExecutor
{
public:
  BlockExecutor(
    const std::vector<StmtPtr>& statements,
    Interpreter& intpr,
    const EnvSPtr& env)
    : statements_(statements)
    , intpr_(intpr)
    , prev_(intpr.environment())  // back up the current env
  {
    // set the new env
    intpr.environment(env);
  }

  // execute statements in block
  void operator()()
  {
    for ( auto& stmt : statements_ ) {
      stmt->execute(intpr_);
    }
  }

  // restore the current env when done or exception was thrown
  ~BlockExecutor()
  {
    intpr_.environment(prev_);
  }

private:
  const std::vector<StmtPtr>& statements_;
  Interpreter& intpr_;
  const EnvSPtr prev_;
};

/*---------------------------------------------------------------------------*/

/** Equality comparison for LoxObject.
 */
bool operator==(const LoxObject& left, const LoxObject& right)
{
  // nil == nil is true
  if ( !left && !right ) return true;

  // short circuit if left is nil
  if ( !left ) return false;

  // compare for literal values
  if ( left && right ) {
    if (
      std::holds_alternative<double>(left.value()) &&
      std::holds_alternative<double>(right.value()) )
      return std::get<double>(left.value()) == std::get<double>(right.value());

    if (
      std::holds_alternative<std::string>(left.value()) &&
      std::holds_alternative<std::string>(right.value()) )
      return std::get<std::string>(left.value()) ==
             std::get<std::string>(right.value());

    if (
      std::holds_alternative<bool>(left.value()) &&
      std::holds_alternative<bool>(right.value()) )
      return std::get<bool>(left.value()) == std::get<bool>(right.value());

    // TODO: support for other types
  }

  return false;
}

/*---------------------------------------------------------------------------*/

Interpreter::Interpreter()
  : env_(std::make_shared<Environment>())
{
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::interpret(Expr& expr)
{
  try {
    return evaluate(expr);
  } catch ( const RuntimeError& error ) {
    YaLox::runtimeError(error);
  }

  return {};
}

/*---------------------------------------------------------------------------*/

/** Execute a Lox program.
 */
void Interpreter::interpret(const std::vector<StmtPtr>& program)
{
  try {
    for ( const auto& stmt : program ) {
      stmt->execute(*this);
    }
  } catch ( const RuntimeError& error ) {
    YaLox::runtimeError(error);
  }
}

/*---------------------------------------------------------------------------*/

/** Get current environment.
 */
const EnvSPtr& Interpreter::environment() const
{
  return env_;
}

/*---------------------------------------------------------------------------*/

/** Set new environment.
 */
void Interpreter::environment(const EnvSPtr& env)
{
  env_ = env;
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::evaluate(Expr& expr)
{
  return expr.evaluate(*this);
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::visitAssignExpr(AssignExpr& expr)
{
  LoxObject value = evaluate(*(expr.value));
  env_->assign(expr.name, value);
  return value;
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::visitBinaryExpr(BinaryExpr& expr)
{
  auto left = evaluate(*(expr.left));
  auto right = evaluate(*(expr.right));

  switch ( expr.op.type() ) {
    case TokenType::MINUS:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) - std::get<double>(right.value());
    case TokenType::PLUS: {
      if ( left && right ) {
        if (
          std::holds_alternative<std::string>(left.value()) &&
          std::holds_alternative<std::string>(right.value()) ) {
          return std::get<std::string>(left.value()) +
                 std::get<std::string>(right.value());
        }
        if (
          std::holds_alternative<double>(left.value()) &&
          std::holds_alternative<double>(right.value()) ) {
          return std::get<double>(left.value()) +
                 std::get<double>(right.value());
        }
      }

      throw RuntimeError(
        expr.op, "Operands must be two numbers or two strings.");
    }

    case TokenType::SLASH:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) / std::get<double>(right.value());
    case TokenType::STAR:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) * std::get<double>(right.value());

    case TokenType::GREATER:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) > std::get<double>(right.value());
    case TokenType::GREATER_EQUAL:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) >= std::get<double>(right.value());
    case TokenType::LESS:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) < std::get<double>(right.value());
    case TokenType::LESS_EQUAL:
      validateNumberOperands(expr.op, left, right);
      return std::get<double>(left.value()) <= std::get<double>(right.value());

    case TokenType::BANG_EQUAL:
      return left != right;
    case TokenType::EQUAL_EQUAL:
      return left == right;

    default:
      break;
  }

  // unreachable
  return {};
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::visitGroupingExpr(GroupingExpr& expr)
{
  return evaluate(*(expr.expression));
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::visitLiteralExpr(LiteralExpr& expr)
{
  return expr.value;
}

/*---------------------------------------------------------------------------*/

/** Evaluate logical expression.
 */
LoxObject Interpreter::visitLogicalExpr(LogicalExpr& expr)
{
  auto left = evaluate(*(expr.left));

  if ( expr.op.type() == TokenType::OR ) {
    // short circuit for OR op
    if ( isTruthy(left) ) return left;
  } else {
    // short circuit for AND op
    if ( !isTruthy(left) ) return left;
  }

  return evaluate(*(expr.right));
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::visitUnaryExpr(UnaryExpr& expr)
{
  auto right = evaluate(*(expr.right));

  switch ( expr.op.type() ) {
    case TokenType::BANG:
      return !isTruthy(right);
    case TokenType::MINUS:
      validateNumberOperand(expr.op, right);
      return -(std::get<double>(right.value()));
    default:
      break;
  }

  // unreachable
  return {};
}

/*---------------------------------------------------------------------------*/

/** Evaluate a variable expression.
 */
LoxObject Interpreter::visitVariableExpr(VariableExpr& expr)
{
  return env_->get(expr.name);
}

/*---------------------------------------------------------------------------*/

/** Execute block statement.
 */
void Interpreter::visitBlockStmt(BlockStmt& stmt)
{
  // execute the block in a new env whose outer scope is the current env.
  BlockExecutor(stmt.statements, *this, std::make_shared<Environment>(env_))();
}

/*---------------------------------------------------------------------------*/

void Interpreter::visitExprStmt(ExprStmt& stmt)
{
  evaluate(*(stmt.expression));
}

/*---------------------------------------------------------------------------*/

/** Execute if statement.
 */
void Interpreter::visitIfStmt(IfStmt& stmt)
{
  if ( isTruthy(evaluate(*(stmt.condition))) ) {
    stmt.thenBranch->execute(*this);
  } else if ( stmt.elseBranch ) {
    stmt.elseBranch->execute(*this);
  }
}

/*---------------------------------------------------------------------------*/

void Interpreter::visitPrintStmt(PrintStmt& stmt)
{
  auto value = evaluate(*(stmt.expression));
  std::cout << toString(value) << '\n';
}

/*---------------------------------------------------------------------------*/

/** Execute the variable declaration statement.
 *
 * If the variable has an initiliazer, we evaluate it. If not, we set it to nil.
 * Then we tell the environment to bind the variable to that value.
 */
void Interpreter::visitVarStmt(VarStmt& stmt)
{
  LoxObject value{};
  if ( stmt.initializer ) {
    value = evaluate(*(stmt.initializer));
  }

  env_->define(stmt.name.lexeme(), value);
}

/*---------------------------------------------------------------------------*/

/** Execute the while statement.
 */
void Interpreter::visitWhileStmt(WhileStmt& stmt)
{
  while ( isTruthy(evaluate(*(stmt.condition))) ) {
    stmt.body->execute(*this);
  }
}

/*---------------------------------------------------------------------------*/

/** Check if a literal is a number for a given operator.
 */
void Interpreter::validateNumberOperand(
  const Token& op,
  const LoxObject& operand) const
{
  if ( operand && std::holds_alternative<double>(operand.value()) ) return;

  throw RuntimeError(op, "Operand must be a number.");
}

/*---------------------------------------------------------------------------*/

/** Check if both literals is a number for a given operator.
 */
void Interpreter::validateNumberOperands(
  const Token& op,
  const LoxObject& left,
  const LoxObject& right) const
{
  if (
    left && right && std::holds_alternative<double>(left.value()) &&
    std::holds_alternative<double>(right.value()) )
    return;

  throw RuntimeError(op, "Operands must be numbers.");
}

/*---------------------------------------------------------------------------*/

/** Determine the truthfulness of a value
 *
 * false and nil are falsey, and everything else is truthy.
 */
bool Interpreter::isTruthy(const LoxObject& obj) const
{
  if ( !obj ) {
    return false;
  }

  if ( std::holds_alternative<bool>(obj.value()) ) {
    return std::get<bool>(obj.value());
  }

  return true;
}

/*---------------------------------------------------------------------------*/

RuntimeError::RuntimeError(const Token& token, const std::string& message)
  : std::runtime_error(message)
  , token(token)
{
}

}
