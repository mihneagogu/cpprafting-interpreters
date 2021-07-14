#include "stmt.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

#include "../util.hpp"

Expression::Expression(Expression &&to_move) : expr(std::move(to_move.expr)) {}

Expression::Expression(Expr expr) : expr(std::move(expr)) {}

Print::Print(Expr expr) : expr(std::move(expr)) {}

Print::Print(Print &&to_move) : expr(std::move(to_move.expr)) {}

Var::Var(Token name, Expr initializer)
    : name(std::move(name)), initializer(std::move(initializer)) {}

Var::Var(Var &&to_move)
    : name(std::move(to_move.name)),
      initializer(std::move(to_move.initializer)) {}

Block::Block(std::vector<Stmt> statements)
    : statements(std::move(statements)) {}

Block::Block(Block &&to_move) : statements(std::move(to_move.statements)) {}

IfStmt::IfStmt(Expr cond, Stmt *then_branch, Stmt *else_branch)
    : condition(std::move(cond)), then_branch(then_branch),
      else_branch(else_branch) {}
IfStmt::IfStmt(IfStmt &&to_move) : condition(std::move(to_move.condition)) {
  this->then_branch = to_move.then_branch;
  this->else_branch = to_move.else_branch;
  to_move.then_branch = nullptr;
  to_move.else_branch = nullptr;
}

IfStmt::~IfStmt() {
  if (this->then_branch != nullptr) {
    delete this->then_branch;
  }
  if (this->else_branch != nullptr) {
    delete this->else_branch;
  }
  // other fields deleted automatically
}

WhileStmt::WhileStmt(Expr cond, Stmt *body): cond(std::move(cond)), body(body) {}
WhileStmt::WhileStmt(WhileStmt &&to_move): cond(std::move(to_move.cond)) {
  this->body = to_move.body;
  to_move.body = nullptr;
}

WhileStmt::~WhileStmt() {
  if (this->body != nullptr) {
    delete this->body;
  }
}

FuncStmt::FuncStmt(Token name, std::vector<Token> params, Block body):
  name(std::move(name)), params(std::move(params)), body(std::move(body))
{}
FuncStmt::FuncStmt(FuncStmt &&to_move):
  name(std::move(to_move.name)), params(std::move(to_move.params)), body(std::move(to_move.body))
{}

Stmt::Stmt(Expression expression) : expression(std::move(expression)) {
  this->ty = StmtTy::STMT_EXPR;
}

Stmt::Stmt(Print print) : print(std::move(print)) {
  this->ty = StmtTy::STMT_PRINT;
}

Stmt::Stmt(Var v) : var(std::move(v)) { this->ty = StmtTy::STMT_VAR; }

Stmt::Stmt(Block block) : block(std::move(block)) {
  this->ty = StmtTy::STMT_BLOCK;
}

Stmt::Stmt(IfStmt if_stmt) : if_stmt(std::move(if_stmt)) {
  this->ty = StmtTy::STMT_IF;
}

Stmt::Stmt(WhileStmt while_stmt): while_stmt(std::move(while_stmt)) {
  this->ty = StmtTy::STMT_WHILE;
}

Stmt::Stmt(FuncStmt func_stmt): func_stmt(std::move(func_stmt)) {
  this->ty = StmtTy::STMT_FUNC;
}

Stmt::Stmt(Stmt &&to_move) {
  this->ty = to_move.ty;
  switch (to_move.ty) {
  case StmtTy::STMT_EXPR:
    init_union_field(this->expression, Expression,
                     std::move(to_move.expression));
    break;
  case StmtTy::STMT_PRINT:
    init_union_field(this->print, Print, std::move(to_move.print));
    break;
  case StmtTy::STMT_VAR:
    init_union_field(this->var, Var, std::move(to_move.var));
    break;
  case StmtTy::STMT_BLOCK:
    init_union_field(this->block, Block, std::move(to_move.block));
    break;
  case StmtTy::STMT_IF:
    init_union_field(this->if_stmt, IfStmt, std::move(to_move.if_stmt));
    break;
  case StmtTy::STMT_WHILE:
    init_union_field(this->while_stmt, WhileStmt, std::move(to_move.while_stmt));
    break;
  case StmtTy::STMT_FUNC:
    init_union_field(this->func_stmt, FuncStmt, std::move(to_move.func_stmt));
    break;
  default:
    std::cerr << "Unknown Statement type. This should never happen"
              << std::endl;
  }
}

Stmt &Stmt::operator=(Stmt &&to_move) {
  switch (this->ty) {
  case StmtTy::STMT_EXPR:
    std::destroy_at(&this->expression);
    break;
  case StmtTy::STMT_PRINT:
    std::destroy_at(&this->print);
    break;
  case StmtTy::STMT_VAR:
    std::destroy_at(&this->var);
    break;
  case StmtTy::STMT_BLOCK:
    std::destroy_at(&this->block);
    break;
  case StmtTy::STMT_IF:
    std::destroy_at(&this->if_stmt);
    break;
  case StmtTy::STMT_WHILE:
    std::destroy_at(&this->while_stmt);
    break;
  case StmtTy::STMT_FUNC:
    std::destroy_at(&this->func_stmt);
    break;
  default:
    throw std::runtime_error(
        "Unknown Statement type when assigning. This should never happen");
  }
  switch (to_move.ty) {
  case StmtTy::STMT_EXPR:
    init_union_field(this->expression, Expression, std::move(expression));
    break;
  case StmtTy::STMT_PRINT:
    init_union_field(this->print, Print, std::move(print));
    break;
  case StmtTy::STMT_VAR:
    init_union_field(this->var, Var, std::move(to_move.var));
    break;
  case StmtTy::STMT_BLOCK:
    init_union_field(this->block, Block, std::move(to_move.block));
    break;
  case StmtTy::STMT_IF:
    init_union_field(this->if_stmt, IfStmt, std::move(to_move.if_stmt));
    break;
  case StmtTy::STMT_WHILE:
    init_union_field(this->while_stmt, WhileStmt, std::move(to_move.while_stmt));
    break;
  case StmtTy::STMT_FUNC:
    init_union_field(this->func_stmt, FuncStmt, std::move(to_move.func_stmt));
    break;
  default:
    throw std::runtime_error(
        "Unknown Statement type when assigning. This should never happen");
  }
  this->ty = to_move.ty;
  return *this;
}

Stmt::~Stmt() {
  switch (this->ty) {
  case StmtTy::STMT_EXPR:
    std::destroy_at(&this->expression);
    break;
  case StmtTy::STMT_PRINT:
    std::destroy_at(&this->print);
    break;
  case StmtTy::STMT_VAR:
    std::destroy_at(&this->var);
    break;
  case StmtTy::STMT_BLOCK:
    std::destroy_at(&this->block);
    break;
  case StmtTy::STMT_IF:
    std::destroy_at(&this->if_stmt);
    break;
  case StmtTy::STMT_WHILE:
    std::destroy_at(&this->while_stmt);
    break;
  case StmtTy::STMT_FUNC:
    std::destroy_at(&this->func_stmt);
    break;
  default:
    std::cerr << "Unknown Statement type. This should never happen"
              << std::endl;
  }
}
