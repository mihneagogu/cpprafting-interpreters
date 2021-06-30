#include <cstdarg>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "../util.hpp"
#include "expr.hpp"
#include "tokens.hpp"

BinaryExpr::BinaryExpr(BinaryExpr &&to_move) : op(std::move(to_move.op)) {
  this->left = to_move.left;
  to_move.left = nullptr;
  this->right = to_move.right;
  to_move.right = nullptr;
}

BinaryExpr &BinaryExpr::operator=(BinaryExpr &&to_move) {
  this->left = to_move.left;
  this->right = to_move.right;
  this->op = std::move(to_move.op);
  to_move.left = nullptr;
  to_move.right = nullptr;
  return *this;
}

BinaryExpr::BinaryExpr(Expr *left, Token op, Expr *right) : op(std::move(op)) {
  this->left = left;
  this->right = right;
}

std::string BinaryExpr::parenthesize() const {
  return Expr::parenthesize(this->op.lexeme, 2, this->left, this->right);
}

BinaryExpr::~BinaryExpr() {
  if (this->left != nullptr) {
    delete this->left;
  }
  if (this->right != nullptr) {
    delete this->right;
  }
}

GroupingExpr::GroupingExpr(Expr *expr) : expression(expr) {}

GroupingExpr::GroupingExpr(GroupingExpr &&to_move) {
  this->expression = to_move.expression;
  to_move.expression = nullptr;
}

GroupingExpr &GroupingExpr::operator=(GroupingExpr &&to_move) {
  this->expression = to_move.expression;
  to_move.expression = nullptr;
  return *this;
}

std::string GroupingExpr::parenthesize() const {
  return Expr::parenthesize("group", 1, this->expression);
}

GroupingExpr::~GroupingExpr() {
  if (this->expression != nullptr) {
    delete this->expression;
  }
}

LiteralExpr::LiteralExpr(Literal lit) : lit(std::move(lit)) {}

LiteralExpr::LiteralExpr(LiteralExpr &&to_move) : lit(std::move(to_move.lit)) {}

LiteralExpr &LiteralExpr::operator=(LiteralExpr &&to_move) {
  this->lit = std::move(to_move.lit);
  return *this;
}

std::string LiteralExpr::parenthesize() const {
  switch (this->lit.ty) {
  case LiteralTy::LIT_NUMBER:
    return std::to_string(this->lit.number);
  case LiteralTy::LIT_STRING:
    return this->lit.str;
  case LiteralTy::LIT_NIL:
    return "nil";
  default:
    return "some_literal";
  }
}

LiteralExpr::~LiteralExpr() = default;

UnaryExpr::UnaryExpr(Token op, Expr *right) : op(std::move(op)) {
  this->right = right;
}

UnaryExpr::UnaryExpr(UnaryExpr &&to_move) : op(std::move(to_move.op)) {
  this->right = to_move.right;
  to_move.right = nullptr;
}

UnaryExpr &UnaryExpr::operator=(UnaryExpr &&to_move) {
  this->op = std::move(to_move.op);
  this->right = to_move.right;
  to_move.right = nullptr;
  return *this;
}

std::string UnaryExpr::parenthesize() const {
  return Expr::parenthesize(this->op.lexeme, 1, this->right);
}

UnaryExpr::~UnaryExpr() {
  if (this->right != nullptr) {
    delete this->right;
  }
}

VariableExpr::VariableExpr(Token name) : name(std::move(name)) {}
VariableExpr::VariableExpr(VariableExpr &&to_move)
    : name(std::move(to_move.name)) {}

std::string VariableExpr::parenthesize() const { return this->name.lexeme; }

AssignExpr::AssignExpr(Token name, Expr *value)
    : name(std::move(name)), value(value) {}
AssignExpr::AssignExpr(AssignExpr &&to_move) : name(std::move(to_move.name)) {
  this->value = to_move.value;
  to_move.value = nullptr;
}

AssignExpr::~AssignExpr() {
  // this->name is automatically cleaned up
  if (this->value != nullptr) {
    delete this->value;
  }
}

LogicalExpr::LogicalExpr(Token op, Expr *left, Expr *right)
    : op(std::move(op)), left(left), right(right) {}

LogicalExpr::LogicalExpr(LogicalExpr &&to_move): op(std::move(to_move.op)) {
  this->left = to_move.left;
  this->right = to_move.right;
  to_move.left = nullptr;
  to_move.right = nullptr;
}

LogicalExpr::~LogicalExpr() {
  if (this->left != nullptr) {
    delete this->left;
  }
  if (this->right != nullptr) {
    delete this->right;
  }
}

Expr::Expr(BinaryExpr bin) {
  this->ty = ExprTy::BINARY;
  init_union_field(this->bin, BinaryExpr, std::move(bin));
}

Expr::Expr(GroupingExpr group) {
  this->ty = ExprTy::GROUPING;
  init_union_field(this->group, GroupingExpr, std::move(group));
}

Expr::Expr(LiteralExpr lit) {
  this->ty = ExprTy::LITERAL;
  init_union_field(this->lit, LiteralExpr, std::move(lit));
}

Expr::Expr(UnaryExpr unary) {
  this->ty = ExprTy::UNARY;
  init_union_field(this->unary, UnaryExpr, std::move(unary));
}

Expr::Expr(VariableExpr var_expr) {
  this->ty = ExprTy::VAR_EXPR;
  init_union_field(this->var_expr, VariableExpr, std::move(var_expr));
}

Expr::Expr(AssignExpr ass_expr) {
  this->ty = ExprTy::ASSIGN_EXPR;
  init_union_field(this->ass_expr, AssignExpr, std::move(ass_expr));
}

Expr::Expr(LogicalExpr logic) {
  this->ty = ExprTy::LOGICAL_EXPR;
  init_union_field(this->logical, LogicalExpr, std::move(logic));
}

Expr &Expr::operator=(Expr &&to_move) {
  // The order here is very important. It might be the case that "this" was
  // previously another type of expression, in which case we need to manually
  // destruct the expression from before (as we are using a union) and only the
  // initialize the union field (without this->, since that would destruct the
  // previous parameter, which is obviously uninited memory)

  switch (this->ty) {
  case ExprTy::BINARY:
    std::destroy_at(&this->bin);
    break;
  case ExprTy::GROUPING:
    std::destroy_at(&this->group);
    break;
  case ExprTy::LITERAL:
    std::destroy_at(&this->lit);
    break;
  case ExprTy::UNARY:
    std::destroy_at(&this->unary);
    break;
  case ExprTy::VAR_EXPR:
    std::destroy_at(&this->var_expr);
    break;
  case ExprTy::ASSIGN_EXPR:
    std::destroy_at(&this->ass_expr);
    break;
  case ExprTy::LOGICAL_EXPR:
    std::destroy_at(&this->logical);
    break;
  default:
    throw std::runtime_error("Unknown Expr type when destructing");
  }
  switch (to_move.ty) {
  case ExprTy::BINARY:
    init_union_field(this->bin, BinaryExpr, std::move(to_move.bin));
    break;
  case ExprTy::GROUPING:
    init_union_field(this->group, GroupingExpr, std::move(to_move.group));
    break;
  case ExprTy::LITERAL:
    init_union_field(this->lit, LiteralExpr, std::move(to_move.lit));
    break;
  case ExprTy::UNARY:
    init_union_field(this->unary, UnaryExpr, std::move(to_move.unary));
    break;
  case ExprTy::VAR_EXPR:
    init_union_field(this->var_expr, VariableExpr, std::move(to_move.var_expr));
    break;
  case ExprTy::ASSIGN_EXPR:
    init_union_field(this->ass_expr, AssignExpr, std::move(to_move.ass_expr));
    break;
  case ExprTy::LOGICAL_EXPR:
    init_union_field(this->logical, LogicalExpr, std::move(to_move.logical));
    break;
  default:
    throw std::runtime_error("Unknown Expr type when destructing");
  }
  this->ty = to_move.ty;
  return *this;
}

Expr::Expr(Expr &&to_move) {
  this->ty = to_move.ty;
  switch (this->ty) {
  case ExprTy::BINARY:
    init_union_field(this->bin, BinaryExpr, std::move(to_move.bin));
    break;
  case ExprTy::GROUPING:
    init_union_field(this->group, GroupingExpr, std::move(to_move.group));
    break;
  case ExprTy::LITERAL:
    init_union_field(this->lit, LiteralExpr, std::move(to_move.lit));
    break;
  case ExprTy::UNARY:
    init_union_field(this->unary, UnaryExpr, std::move(to_move.unary));
    break;
  case ExprTy::VAR_EXPR:
    init_union_field(this->var_expr, VariableExpr, std::move(to_move.var_expr));
    break;
  case ExprTy::ASSIGN_EXPR:
    init_union_field(this->ass_expr, AssignExpr, std::move(to_move.ass_expr));
    break;
  case ExprTy::LOGICAL_EXPR:
    init_union_field(this->logical, LogicalExpr, std::move(to_move.logical));
    break;
  default:
    std::cerr << "Unknown Literal type when constructing an Expr. This should "
                 "never happen"
              << std::endl;
  }
}

Expr::~Expr() {
  switch (this->ty) {
  case ExprTy::BINARY:
    std::destroy_at(&this->bin);

  case ExprTy::GROUPING:
    std::destroy_at(&this->group);
    break;
  case ExprTy::LITERAL:
    std::destroy_at(&this->lit);
    break;
  case ExprTy::UNARY:
    std::destroy_at(&this->unary);
    break;
  case ExprTy::VAR_EXPR:
    std::destroy_at(&this->var_expr);
    break;
  case ExprTy::ASSIGN_EXPR:
    std::destroy_at(&this->ass_expr);
    break;
  case ExprTy::LOGICAL_EXPR:
    std::destroy_at(&this->logical);
    break;
  default:
    std::cerr << "Unknown Expression type. This should never happen"
              << std::endl;
    break;
  }
}

std::string Expr::parenthesize() const {
  switch (this->ty) {
  case ExprTy::BINARY:
    return this->bin.parenthesize();
  case ExprTy::GROUPING:
    return this->group.parenthesize();
  case ExprTy::LITERAL:
    return this->lit.parenthesize();
  case ExprTy::UNARY:
    return this->unary.parenthesize();
  case ExprTy::VAR_EXPR:
    return this->var_expr.name.lexeme;
  case ExprTy::ASSIGN_EXPR:
    // TODO: Implement parenthesize for assign_expr
    return "(some_assignment)";
  case ExprTy::LOGICAL_EXPR:
    return "(some_logical_expr)";
  default:
    throw std::runtime_error(
        "Unknown expression type. This should never happen");
  }
}

Expr Expr::lox_nil() { return Expr(LiteralExpr(Literal::lox_nil())); }

bool Expr::is_nil() const {
  if (this->ty == ExprTy::LITERAL && this->lit.lit.is_nil()) {
    return true;
  }
  return false;
}

std::string Expr::parenthesize(const std::string &name, int n_args, ...) {

  va_list args;
  va_start(args, n_args);

  std::string res = "";
  res += '(';
  res += name;

  for (int i = 0; i < n_args; i++) {
    const Expr *e = va_arg(args, const Expr *);
    res += ' ';
    res += e->parenthesize();
  }
  res += ')';
  return res;
}
