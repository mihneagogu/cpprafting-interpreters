#ifndef EXPR_H_
#define EXPR_H_

#include <string>

#include "tokens.hpp"
#include "../util.hpp"

class Expr;

class BinaryExpr {
  public:
    Expr* left;
    Token op/*erator*/;
    Expr* right;
    std::string parenthesize() const;
    BinaryExpr(Expr *left, Token op, Expr *right);
    BinaryExpr(BinaryExpr &&to_move);
    BinaryExpr& operator=(BinaryExpr &&to_move);
    ~BinaryExpr();
};

class GroupingExpr {
  public:
    Expr* expression;
    std::string parenthesize() const;
    GroupingExpr(Expr *expr);
    GroupingExpr(GroupingExpr &&to_move);
    GroupingExpr& operator=(GroupingExpr &&to_move);
    ~GroupingExpr();
};

class LiteralExpr {
  public:
    Literal lit;
    std::string parenthesize() const;
    LiteralExpr(Literal lit);
    LiteralExpr(LiteralExpr &&to_move);
    LiteralExpr& operator=(LiteralExpr &&to_move);
    ~LiteralExpr();
};

class UnaryExpr {
  public:
    Token op/*erator*/;
    Expr* right;
    std::string parenthesize() const;
    UnaryExpr(Token op, Expr *right);
    UnaryExpr(UnaryExpr &&to_move);
    UnaryExpr& operator=(UnaryExpr &&to_move);
    ~UnaryExpr();
};

class VariableExpr {
    public:
        Token name;
        std::string parenthesize() const;
    VariableExpr(Token name);
    VariableExpr(VariableExpr &&to_move);
    ~VariableExpr() = default;
};


class AssignExpr {
    public:
        Token name;
        Expr *value;
        AssignExpr(Token name, Expr *value);
        AssignExpr(AssignExpr &&to_move);
        ~AssignExpr();
};


enum ExprTy {
    BINARY, GROUPING, LITERAL, UNARY, VAR_EXPR, ASSIGN_EXPR
};

class Expr {
    public:
        ExprTy ty;
        union {
            BinaryExpr bin;
            GroupingExpr group;
            LiteralExpr lit;
            UnaryExpr unary;
            VariableExpr var_expr;
            AssignExpr ass_expr;
        };
    static Expr lox_nil();
    Expr(BinaryExpr bin);
    Expr(GroupingExpr group);
    Expr(LiteralExpr lit);
    Expr(UnaryExpr unary);
    Expr(VariableExpr var_expr);
    Expr(AssignExpr ass_expr);
    Expr(Expr&& to_move);
    Expr& operator=(Expr&& to_move);
    ~Expr();
    bool is_nil() const;

    std::string parenthesize() const;
    /* Parenthesizes a list of expressions. The type of each element of ... is "Expr *" */
    static std::string parenthesize(const std::string& name, int n_args, ...);
};



#endif // EXPR_H_
