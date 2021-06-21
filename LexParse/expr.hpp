#ifndef EXPR_H_
#define EXPR_H_

#include <string>

#include "tokens.hpp"

class Expr;

class BinaryExpr {
  public:
    Expr* left;
    Token op/*erator*/;
    Expr* right;
    std::string parenthesize() const;
    BinaryExpr(Expr *left, Token op, Expr *right);
    BinaryExpr(BinaryExpr &&to_move);
};

class GroupingExpr {
  public:
    Expr* expression;
    std::string parenthesize() const;
    GroupingExpr(Expr *expr);
    GroupingExpr(GroupingExpr &&to_move);
};

class LiteralExpr {
  public:
    void* maybe_value;
    std::string parenthesize() const;
    LiteralExpr(void *maybe_value);
    LiteralExpr(LiteralExpr &&to_move);
};

class UnaryExpr {
  public:
    Token op/*erator*/;
    Expr* right;
    std::string parenthesize() const;
    UnaryExpr(Token op, Expr *right);
    UnaryExpr(UnaryExpr &&to_move);
};


enum ExprTy {
    BINARY, GROUPING, LITERAL, UNARY
};

class Expr {
    public:
        ExprTy ty;
        union {
            BinaryExpr bin;
            GroupingExpr group;
            LiteralExpr lit;
            UnaryExpr unary;
        } content;

    Expr(BinaryExpr bin);
    Expr(GroupingExpr group);
    Expr(LiteralExpr lit);
    Expr(UnaryExpr unary);

    std::string parenthesize() const;
    /* Parenthesizes a list of expressions. The type of each element of ... is "Expr *" */
    static std::string parenthesize(std::string name, int n_args, ...);
};



#endif // EXPR_H_
