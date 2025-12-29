#include "interpreter.hpp"

#include "yalox.hpp"

namespace lox {

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

LoxObject Interpreter::evaluate(Expr& expr)
{
  return expr.evaluate(*this);
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
      if (
        std::holds_alternative<std::string>(left.value()) &&
        std::holds_alternative<std::string>(right.value()) ) {
        return std::get<std::string>(left.value()) +
               std::get<std::string>(right.value());
      }
      if (
        std::holds_alternative<double>(left.value()) &&
        std::holds_alternative<double>(right.value()) ) {
        return std::get<double>(left.value()) + std::get<double>(right.value());
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

LoxObject Interpreter::visitUnaryExpr(UnaryExpr& expr)
{
  auto right = evaluate(*(expr.right));

  switch ( expr.op.type() ) {
    case TokenType::BANG:
      return !isTruthy(right.value());
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

/** Check if a literal is a number for a given operator.
 */
void Interpreter::validateNumberOperand(
  const Token& op,
  const LoxObject& operand) const
{
  if ( std::holds_alternative<double>(operand.value()) ) return;

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
    std::holds_alternative<double>(left.value()) &&
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
