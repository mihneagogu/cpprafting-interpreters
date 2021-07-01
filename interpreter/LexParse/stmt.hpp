#ifndef STMT_H_
#define STMT_H_

#include "expr.hpp"

#include <vector>

class Stmt;

class Expression {
public:
  Expr expr;
  Expression(Expr expr);
  Expression(Expression &&to_move);
};

class Print {
public:
  Expr expr;
  Print(Expr expr);
  Print(Print &&to_move);
};

class Var {
public:
    Token name;
    Expr initializer;
    Var(Token name, Expr initializer);
    Var(Var &&to_move);
    ~Var() = default;
};

class Block {
  public:
    std::vector<Stmt> statements;
    Block(std::vector<Stmt> statements);
    Block(Block &&to_move);
    ~Block() = default;
};

class IfStmt {
  public:
    Expr condition;
    Stmt *then_branch;
    Stmt *else_branch;
    IfStmt(Expr condition, Stmt *then_branch, Stmt *else_branch);
    IfStmt(IfStmt &&to_move);
    ~IfStmt();
};

class WhileStmt {
  public:
    Expr cond;
    Stmt *body;
    WhileStmt(Expr condition, Stmt *body);
    WhileStmt(WhileStmt &&to_move);
    ~WhileStmt();
};

enum StmtTy { STMT_EXPR, STMT_PRINT, STMT_VAR, STMT_BLOCK, STMT_IF, STMT_WHILE };
class Stmt {
private:
public:
  StmtTy ty;
  union {
    Expression expression;
    Print print;
    Var var;
    Block block;
    IfStmt if_stmt;
    WhileStmt while_stmt;
  };

  Stmt& operator=(Stmt &&to_move);
  Stmt(Expression expression);
  Stmt(Print print);
  Stmt(Var var);
  Stmt(Stmt &&to_move);
  Stmt(Block block);
  Stmt(IfStmt if_stmt);
  Stmt(WhileStmt while_stmt);
  ~Stmt();
};

#endif // STMT_H_
