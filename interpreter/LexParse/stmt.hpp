#ifndef STMT_H_
#define STMT_H_

#include "expr.hpp"

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

enum StmtTy { STMT_EXPR, STMT_PRINT, STMT_VAR };
class Stmt {
private:
public:
  StmtTy ty;
  union {
    Expression expression;
    Print print;
    Var var;
  };
  Stmt& operator=(Stmt &&to_move);
  Stmt(Expression expression);
  Stmt(Print print);
  Stmt(Var var);
  Stmt(Stmt &&to_move);
  ~Stmt();
};

#endif // STMT_H_
