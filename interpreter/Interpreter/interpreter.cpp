#include "interpreter.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../util.hpp"

LoxElement::LoxElement(LoxTy _nil) {
  ASSERT_COND(
      _nil == LoxTy::LOX_NIL,
      "LoxElement nil constructor should only be called with LOX_NIL\n");
  this->ty = LoxTy::LOX_NIL; this->lox_nil = this->ty;
}

LoxElement::LoxElement(double num) {
  this->ty = LoxTy::LOX_NUMBER;
  this->lox_number = num;
}

LoxElement::LoxElement(bool b) {
  this->ty = LoxTy::LOX_BOOL;
  this->lox_bool = b;
}

LoxElement::LoxElement(std::string str) {
  this->ty = LoxTy::LOX_STRING;
  init_union_field(this->lox_str, std::string, std::move(str));
}

LoxElement::LoxElement(LoxElement &&to_move) : ty(to_move.ty) {
  switch (to_move.ty) {
  case LoxTy::LOX_NUMBER:
    this->lox_number = to_move.lox_number;
    break;
  case LoxTy::LOX_STRING:
    init_union_field(this->lox_str, std::string, std::move(to_move.lox_str));
    break;
  case LoxTy::LOX_NIL:
    this->lox_nil = LoxTy::LOX_NIL;
    break;
  case LoxTy::LOX_BOOL:
    this->lox_number = to_move.lox_number;
    break;
  case LoxTy::LOX_OBJ:
    break;
  default:
    std::cerr << "Unknown LoxElement type. This should never happen"
              << std::endl;
  }
}

LoxElement LoxElement::nil() { return LoxElement(LoxTy::LOX_NIL); }

LoxElement::~LoxElement() {
  switch (this->ty) {
  case LoxTy::LOX_STRING:
    std::destroy_at(&this->lox_str);
    break;
  case LoxTy::LOX_OBJ:
  case LoxTy::LOX_BOOL:
  case LoxTy::LOX_NUMBER:
  case LoxTy::LOX_NIL:
    break;
  default:
    std::cerr << "Unknown LoxElement type. This should never happen"
              << std::endl;
    break;
  }
}

LoxElement &LoxElement::operator=(LoxElement &&to_move) {
  switch (this->ty) {
  case LoxTy::LOX_STRING:
    std::destroy_at(&this->lox_str);
    break;
  case LoxTy::LOX_OBJ:
  case LoxTy::LOX_BOOL:
  case LoxTy::LOX_NUMBER:
  case LoxTy::LOX_NIL:
    break;
  default:
    std::cerr << "Unknown LoxElement type. This should never happen"
              << std::endl;
    break;
  }

  switch (to_move.ty) {
  case LoxTy::LOX_STRING:
    this->lox_str = std::move(to_move.lox_str);
    break;
  case LoxTy::LOX_OBJ:
  case LoxTy::LOX_BOOL:
    this->lox_bool = to_move.lox_bool;
    break;
  case LoxTy::LOX_NUMBER:
    this->lox_number = to_move.lox_number;
    break;
  case LoxTy::LOX_NIL:
    this->lox_nil = LoxTy::LOX_NIL;
    break;
  default:
    std::cerr << "Unknown LoxElement type. This should never happen"
              << std::endl;
    break;
  }
  this->ty = to_move.ty;
  return *this;
}

/*
** Constructs a LoxElement from the given literal. We make the distinction
*between
** LiteralTy and LoxTy because it keeps the interpreter clear and the
*interpreting stage
*/
LoxElement Interpreter::evaluate_literal(const Literal &literal) {
  switch (literal.ty) {
  case LiteralTy::LIT_BOOL:
    return LoxElement(literal.lox_bool);
  case LiteralTy::LIT_NIL:
    return LoxElement::nil();
  case LiteralTy::LIT_NUMBER:
    return LoxElement(literal.number);
  case LiteralTy::LIT_STRING:
    return LoxElement(literal.str);
  default:
    throw std::runtime_error("Unknown Literal type. This should never happen");
  }
}

LoxElement Interpreter::evaluate_unary_expr(const UnaryExpr &unary) {
  auto right = evaluate(*unary.right);
  switch (unary.op.type) {
  case TokenType::MINUS:
    check_number_operand(unary.op, right);
    return LoxElement(-right.as_number());
  case TokenType::BANG:
    check_bool_operand(unary.op, right);
    return LoxElement(!right.is_truthy());
    break;
  default:
    throw std::runtime_error(
        "Unknown unary operator when evaluating expression.");
    break;
  }

  return right;
}

bool LoxElement::is_number() const { return is_instance_of(LoxTy::LOX_NUMBER); }

double LoxElement::as_number() const {
  if (this->ty == LoxTy::LOX_NUMBER) {
    return this->lox_number;
  }
  throw std::runtime_error("LoxElement isn't a number!");
}

bool LoxElement::equals(const LoxElement &other) {
  if (is_nil() && other.is_nil()) {
    return true;
  } // Two nils are equal
  if (is_nil() || other.is_nil()) {
    return false;
  } // If only one of them is nil, they are not equal
  if (this->ty != other.ty) {
    return false;
  }
  switch (this->ty) {
  case LoxTy::LOX_NUMBER:
    return this->lox_number == other.lox_number;
  case LoxTy::LOX_BOOL:
    return this->lox_bool == other.lox_bool;
  case LoxTy::LOX_STRING:
    return this->lox_str == other.lox_str;
  case LoxTy::LOX_OBJ:
    // TODO
    return false;
  default:
    std::cerr << "Unknown LoxElement type. This should never happen"
              << std::endl;
    return false;
  }
}

LoxElement Interpreter::evaluate_binary_expr(const BinaryExpr &binary) {
  auto left = evaluate(*binary.left);
  auto right = evaluate(*binary.right);

  switch (binary.op.type) {
  case TokenType::MINUS:
    // Perform binop on exprs and check that they are the right type
    check_number_operands(binary.op, left, right);
    return LoxElement(left.as_number() - right.as_number());
    break;
  case TokenType::SLASH:
    // Perform binop on exprs and check that they are the right type
    check_number_operands(binary.op, left, right);
    if (right.as_number() == 0.0) {
      throw DivisionByZeroErr{};
    }
    return LoxElement(left.as_number() / right.as_number());
  case TokenType::STAR:
    // Perform binop on exprs and check that they are the right type
    check_number_operands(binary.op, left, right);
    return LoxElement(left.as_number() * right.as_number());
    break;
  case TokenType::PLUS:
    if (left.is_number() && right.is_number()) {
      return LoxElement(left.as_number() + right.as_number());
    }
    if (left.is_instance_of(LoxTy::LOX_STRING) &&
        right.is_instance_of(LoxTy::LOX_STRING)) {
      std::string res = left.lox_str;
      res += right.lox_str;
      return LoxElement(std::move(res));
    }
    throw LoxRuntimeErr{};
  case TokenType::GREATER:
    check_number_operands(binary.op, left, right);
    return LoxElement(left.as_number() > right.as_number());
  case TokenType::GREATER_EQUAL:
    check_number_operands(binary.op, left, right);
    return LoxElement(left.as_number() >= right.as_number());
  case TokenType::LESS:
    check_number_operands(binary.op, left, right);
    return LoxElement(left.as_number() < right.as_number());
  case TokenType::LESS_EQUAL:
    check_number_operands(binary.op, left, right);
    return LoxElement(left.as_number() <= right.as_number());
  case TokenType::BANG_EQUAL:
    return !left.equals(right);
  case TokenType::EQUAL_EQUAL:
    return left.equals(right);
  default:
    std::cerr << "Unknown binary operator. This should never happen"
              << std::endl;
    break;
  }
  UNREACHABLE();
}

LoxElement Interpreter::evaluate_grouping_expr(const GroupingExpr &group) {
  return evaluate(*group.expression);
}
bool Interpreter::check_number_operand(const Token &tok,
                                       const LoxElement &right) {
  if (right.is_number()) {
    return true;
  }
  throw LoxRuntimeErr{};
}

bool Interpreter::check_bool_operand(const Token &tok,
                                     const LoxElement &right) {
  if (right.ty != LoxTy::LOX_BOOL) {
    throw LoxRuntimeErr{};
  }
  return true;
}
bool Interpreter::check_number_operands(const Token &tok,
                                        const LoxElement &left,
                                        const LoxElement &right) {
  if (left.ty != LoxTy::LOX_NUMBER || right.ty != LoxTy::LOX_NUMBER) {
    throw LoxRuntimeErr{};
  }
  return true;
}

bool LoxElement::is_instance_of(LoxTy ty) const { return this->ty == ty; }

bool LoxElement::is_nil() const { return this->ty == LoxTy::LOX_NIL; }

bool LoxElement::is_truthy() const {
  if (is_instance_of(LoxTy::LOX_BOOL)) {
    return this->lox_bool;
  }
  if (is_nil()) {
    return false;
  }
  return true;
}

std::string LoxElement::stringify() const {
  switch (this->ty) {
    case LoxTy::LOX_STRING: 
      return this->lox_str;
    case LoxTy::LOX_NUMBER:
      return std::to_string(this->lox_number);
    case LoxTy::LOX_BOOL:
      return std::to_string(this->lox_bool);
    case LoxTy::LOX_NIL:
      return "nil";
    case LoxTy::LOX_OBJ:
      UNREACHABLE();
    default:
      throw std::runtime_error("Unknown LoxElement type. This should never happen");
  }
}

LoxElement Interpreter::evaluate(const Expr &expr) {
  switch (expr.ty) {
  case ExprTy::BINARY:
    return evaluate_binary_expr(expr.bin);
  case ExprTy::UNARY:
    return evaluate_unary_expr(expr.unary);
  case ExprTy::GROUPING:
    return evaluate_grouping_expr(expr.group);
  case ExprTy::LITERAL:
    return evaluate_literal(expr.lit.lit);
  default:
    throw std::runtime_error(
        "Unknown expression type when interpreting. This should never happen");
  }
}


void Interpreter::run_print_stmt(const Print &print) {
  auto returned = evaluate(print.expr);
  std::cout << returned.stringify() << std::endl;
}

void Interpreter::run_expression_stmt(const Expression &expression) {
  evaluate(expression.expr);
}

void Interpreter::execute(const Stmt &stmt) {
  switch (stmt.ty) {
    case StmtTy::STMT_EXPR:
      run_expression_stmt(stmt.expression);
      break;
    case StmtTy::STMT_PRINT:
      run_print_stmt(stmt.print);
      break;
    default:
      throw std::runtime_error("Unknown Statement type when executing. This should never happen");
  }
}

void Interpreter::interpret(const std::vector<Stmt> &statements) {
  try {
    for (auto &stmt : statements) {
      execute(stmt);
    }
  } catch (LoxRuntimeErr &ler) {
  }
}
