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

enum StmtTy { STMT_EXPR, STMT_PRINT, STMT_VAR, STMT_BLOCK };
class Stmt {
private:
public:
  StmtTy ty;
  union {
    Expression expression;
    Print print;
    Var var;
    Block block;
  };
  Stmt& operator=(Stmt &&to_move);
  Stmt(Expression expression);
  Stmt(Print print);
  Stmt(Var var);
  Stmt(Stmt &&to_move);
  Stmt(Block block);
  ~Stmt();
};

#endif // STMT_H_
