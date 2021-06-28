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

enum StmtTy { STMT_EXPR, STMT_PRINT };
class Stmt {
private:
public:
  StmtTy ty;
  union {
    Expression expression;
    Print print;
  };
  Stmt& operator=(Stmt &&to_move);
  Stmt(Expression expression);
  Stmt(Print print);
  Stmt(Stmt &&to_move);
  ~Stmt();
};

#endif // STMT_H_
