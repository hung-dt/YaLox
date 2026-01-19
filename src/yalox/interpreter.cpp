#include "interpreter.hpp"
#include "yalox.hpp"

#include <chrono>
#include <cassert>
#include <format>
#include <iostream>

namespace lox {

/*---------------------------------------------------------------------------*/

/** Make sure interpreter's env_ does not change after and exit a block.
 *
 * Keep the current env when constructing and restore it when destructing.
 * This is used by executeBlock() to ensure that if the function is done or
 * there is any exception, then the env will not be changed.
 */
class EnvBlockGuard
{
public:
  // Pass the reference of the current env pointer
  EnvBlockGuard(EnvPtr& env)
    : env_(env)
  {
    original_ = env;  // keep the original env pointer address
  }

  ~EnvBlockGuard()
  {
    env_ = original_;  // now restore the original env pointer
  }

  EnvBlockGuard(const EnvBlockGuard&) = delete;
  EnvBlockGuard& operator=(const EnvBlockGuard&) = delete;

private:
  EnvPtr& env_;
  EnvPtr original_;
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

/** The built-in clock() function.
 * Return the number of seconds (with fractional) that have passed since epoch.
 */
LoxObject clockFunc(const std::vector<LoxObject>& /* unused */)
{
  std::chrono::duration<double> duration =
    std::chrono::system_clock::now().time_since_epoch();
  return duration.count();
}

/*---------------------------------------------------------------------------*/

/** The env_ field in the interpreter changes as we enter and exit local
 * scopes. It tracks the current environment. This new globals field holds a
 * fixed reference to the outermost global environment.
 */

Interpreter::Interpreter()
  : globals{ new Environment{} }
{
  env_ = globals;

  // Built-in clock() function
  globals->define("clock", LoxCallable{ 0, nullptr, clockFunc, "<native fn>" });
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
void Interpreter::interpret(std::vector<StmtPtr> statements)
{
  try {
    for ( auto& stmt : statements ) {
      stmt->execute(*this);

      // Store statements so that function/class declaration statement pointers
      // persist to be referenced later when calling them.
      funcStmts_.emplace_back(std::move(stmt));
    }
  } catch ( const RuntimeError& error ) {
    YaLox::runtimeError(error);
  }
}

/*---------------------------------------------------------------------------*/

/** Each time the resolver visits a variable, it tells the interpreter how many
 * scopes there are between the current scope and the scope where the variable
 * is defined. At runtime, this corresponds exactly to the number of
 * environments between the current one and the enclosing one where the
 * interpreter can find the variable’s value. The resolver hands that number to
 * the interpreter by calling this function.
 */
void Interpreter::resolve(Expr& expr, size_t depth)
{
  locals_[&expr] = depth;
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

  if ( const auto it = locals_.find(&expr); it != locals_.end() ) {
    env_->assignAt(it->second, expr.name, value);
  } else {
    globals->assign(expr.name, value);
  }

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

/** Check if a LoxObject is callable.
 */
void validateLoxCallable(Token op, const LoxObject& obj)
{
  if ( obj && std::holds_alternative<LoxCallable>(obj.value()) ) return;

  throw RuntimeError(op, "Can only call functions and classes.");
}

/*---------------------------------------------------------------------------*/

/** Check that the number of passed arguments must match that of function's.
 */
void validateFunctionArity(
  const Token& op,
  const std::vector<LoxObject>& args,
  const LoxCallable& func)
{
  if ( args.size() == func.arity ) return;

  throw RuntimeError(
    op,
    std::format("Expected {} arguments but got {}.", func.arity, args.size()));
}

/*---------------------------------------------------------------------------*/

/** Execute function call.
 */
LoxObject Interpreter::visitCallExpr(CallExpr& expr)
{
  LoxObject callee = evaluate(*(expr.callee));

  std::vector<LoxObject> arguments{};
  for ( auto& arg : expr.arguments ) {
    arguments.emplace_back(evaluate(*arg));
  }

  validateLoxCallable(expr.closingParen, callee);
  auto function = std::get<LoxCallable>(callee.value());

  validateFunctionArity(expr.closingParen, arguments, function);

  return function.call(arguments);
}

/*---------------------------------------------------------------------------*/

/** Access class instance's properties (fields or methods).
 */
LoxObject Interpreter::visitGetExpr(GetExpr& expr)
{
  LoxObject object = evaluate(*(expr.object));
  if ( object && std::holds_alternative<LoxInstancePtr>(object.value()) ) {
    auto& prop = std::get<LoxInstancePtr>(object.value())->get(expr.name);
    if ( prop && std::holds_alternative<LoxCallable>(prop.value()) ) {
      auto& func = std::get<LoxCallable>(prop.value());
      // Bind "this" to the current instance for methods
      bindInstance(func, object);
    }
    return prop;
  }

  throw RuntimeError(expr.name, "Only instances have properties.");
}

/*---------------------------------------------------------------------------*/

/** Create a new environment nestled inside the method's original closure. We
 * declare "this" as a variable in that environment and bind it to the given
 * instance, the instance that the method is being accessed from. When the
 * method is called, that will become the parent of the method's body
 * environment.
 */
void Interpreter::bindInstance(LoxCallable& func, const LoxObject& instance)
{
  EnvPtr methodEnv{ new Environment{ env_ } };
  methodEnv->define("this", instance);
  func = makeLoxCallable(
    *(func.funcStmt), methodEnv, func.funcStmt->name.lexeme() == "init");
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

/** We evaluate the object whose property is being set and check to see if it’s
 * a LoxInstance. If not, that’s a runtime error. Otherwise, we evaluate the
 * value being set and store it on the instance.
 */
LoxObject Interpreter::visitSetExpr(SetExpr& expr)
{
  LoxObject object = evaluate(*(expr.object));

  if ( object && std::holds_alternative<LoxInstancePtr>(object.value()) ) {
    auto value = evaluate(*(expr.value));
    std::get<LoxInstancePtr>(object.value())->set(expr.name, value);
    // we need to reassign the object to the locals_ scope or to the environment
    const auto it = locals_.find(expr.object.get());
    if ( it != locals_.end() ) {
      env_->assignAt(
        it->second, static_cast<VariableExpr&>(*(expr.object)).name, object);
    } else {
      globals->assign(static_cast<VariableExpr&>(*(expr.object)).name, object);
    }
    return value;
  }

  throw RuntimeError(expr.name, "Only instance have fields.");
}

/*---------------------------------------------------------------------------*/

/** Evaluate a "this" expression and return the bound instance for the current
 * class.
 *
 * This function looks up the instance referenced by the 'this' keyword in the
 * interpreter's environment/resolution context and returns it as a LoxObject.
 * It ensures that "this" is used only within a valid class instance context.
 */
LoxObject Interpreter::visitThisExpr(ThisExpr& expr)
{
  return lookUpVariable(expr.keyword, expr);
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
  return lookUpVariable(expr.name, expr);
}

/*---------------------------------------------------------------------------*/

LoxObject Interpreter::lookUpVariable(const Token& name, Expr& expr)
{
  if ( const auto it = locals_.find(&expr); it != locals_.end() ) {
    return env_->getAt(it->second, name.lexeme());
  } else {
    return globals->get(name);
  }
}

/*---------------------------------------------------------------------------*/

void Interpreter::executeBlock(
  const std::vector<StmtPtr>& block,
  EnvPtr& blockEnv)
{
  EnvBlockGuard eg{ this->env_ };

  this->env_ = blockEnv;

  for ( auto& stmt : block ) {
    stmt->execute(*this);
  }
}

/*---------------------------------------------------------------------------*/

/** Execute block statement.
 */
void Interpreter::visitBlockStmt(BlockStmt& stmt)
{
  // execute the block in a new env whose outer scope is the current env.
  EnvPtr blockEnv{ new Environment{ this->env_ } };
  executeBlock(stmt.statements, blockEnv);
}

/*---------------------------------------------------------------------------*/

/** When a class is called, it instantiates a new LoxInstance for the called
 * class and returns it.
 *
 * Class's instance can be initialized with init() method and provided
 * arguments: var obj = MyClass( arg1, arg2 )
 */
void Interpreter::visitClassStmt(ClassStmt& stmt)
{
  LoxCallable lc;
  lc.arity = 0;
  lc.name = stmt.name.lexeme();

  // Gather methods
  for ( auto& methodStmt : stmt.methods ) {
    auto method = static_cast<FunctionStmt*>(methodStmt.get());
    lc.methods.emplace(
      method->name.lexeme(),
      makeLoxCallable(*method, env_, method->name.lexeme() == "init"));
  }

  // Arguments for init() method
  if ( auto it = lc.methods.find("init"); it != lc.methods.end() ) {
    lc.arity = std::get<LoxCallable>(it->second.value()).arity;
  }

  lc.call = [this,
             lc](const std::vector<LoxObject>& args) mutable -> LoxObject {
    auto instance =
      LoxInstancePtr(new LoxInstance{ lc, lc.name + " instance" });

    // Look for init() method and execute it to initialize a class's instance
    if ( auto it = lc.methods.find("init"); it != lc.methods.end() ) {
      auto& initFunc = std::get<LoxCallable>(it->second.value());
      bindInstance(initFunc, instance);
      initFunc.call(args);
    }
    return instance;
  };

  env_->define(stmt.name.lexeme(), std::move(lc));
}

/*---------------------------------------------------------------------------*/

void Interpreter::visitExprStmt(ExprStmt& stmt)
{
  evaluate(*(stmt.expression));
}

/*---------------------------------------------------------------------------*/

/** Execute function declaration statement.
 */
void Interpreter::visitFunctionStmt(FunctionStmt& stmt)
{
  env_->define(stmt.name.lexeme(), makeLoxCallable(stmt, env_, false));
}

/*---------------------------------------------------------------------------*/

LoxCallable Interpreter::makeLoxCallable(
  FunctionStmt& funcStmt,
  const EnvPtr& closure,
  bool isInit)
{
  auto func = &funcStmt;

  LoxCallable lc;
  lc.arity = func->params.size();
  lc.funcStmt = func;
  lc.call = [this, func, closure, isInit](
              const std::vector<LoxObject>& args) -> LoxObject {
    assert(func->params.size() == args.size());

    EnvPtr funcEnv{ new Environment{ closure } };

    for ( size_t i = 0; i < func->params.size(); ++i ) {
      funcEnv->define(func->params[i].lexeme(), args[i]);
    };

    try {
      this->executeBlock(func->body, funcEnv);
    } catch ( const ReturnValue& ret ) {
      // empty early return returns "this" instead of nil
      if ( isInit ) return closure->getAt(0, "this");

      return ret.value;
    }

    // directly call init() always return this
    if ( isInit ) return closure->getAt(0, "this");

    return {};
  };

  lc.name = "<fn " + func->name.lexeme() + ">";

  return lc;
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

/** The built-in print function.
 */
void Interpreter::visitPrintStmt(PrintStmt& stmt)
{
  auto value = evaluate(*(stmt.expression));
  std::cout << toString(value) << '\n';
}

/*---------------------------------------------------------------------------*/

void Interpreter::visitReturnStmt(ReturnStmt& stmt)
{
  LoxObject value{};
  if ( stmt.value ) {
    value = evaluate(*(stmt.value));
  }

  throw ReturnValue(value);
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

  env_->define(stmt.name.lexeme(), std::move(value));
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

/** Execute the for statement as a while loop.
 */
void Interpreter::visitForStmt(ForStmt& stmt)
{
  if ( stmt.initializer ) {
    stmt.initializer->execute(*this);
  }

  while ( true ) {
    if ( stmt.condition ) {
      auto condition = evaluate(*(stmt.condition));
      if ( !isTruthy(condition) ) break;
    }

    // Lox requires a body in for loop so no need to check for null here
    assert(stmt.body);
    stmt.body->execute(*this);

    if ( stmt.increment ) {
      evaluate(*(stmt.increment));
    }
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

ReturnValue::ReturnValue(LoxObject value)
  : std::runtime_error("")
  , value(std::move(value))
{
}

/*---------------------------------------------------------------------------*/

RuntimeError::RuntimeError(const Token& token, const std::string& message)
  : std::runtime_error(message)
  , token(token)
{
}

}
