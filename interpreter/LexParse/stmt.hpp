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

class FuncStmt {
  public:
    Token name;
    std::vector<Token> params;
    Block body;
    FuncStmt(Token name, std::vector<Token> params, Block body);
    FuncStmt(FuncStmt &&to_move);
    ~FuncStmt() = default;
};

class ReturnStmt {
  public:
    Token keyword;
    Expr value;

    ReturnStmt(Token keyword, Expr value);
    ReturnStmt(ReturnStmt &&to_move);
    ~ReturnStmt() = default;
};


enum StmtTy { STMT_EXPR, STMT_PRINT, STMT_VAR, STMT_BLOCK, STMT_IF, STMT_WHILE, STMT_FUNC, STMT_RETURN };
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
    FuncStmt *func_stmt;
    ReturnStmt return_stmt;
  };

  Stmt& operator=(Stmt &&to_move);
  Stmt(Expression expression);
  Stmt(Print print);
  Stmt(Var var);
  Stmt(Stmt &&to_move);
  Stmt(Block block);
  Stmt(IfStmt if_stmt);
  Stmt(WhileStmt while_stmt);
  Stmt(FuncStmt *func_stmt);
  Stmt(ReturnStmt return_stmt);
  ~Stmt();
};

#endif // STMT_H_
