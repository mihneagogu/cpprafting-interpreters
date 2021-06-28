#include "stmt.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>


#include "../util.hpp"


Expression::Expression(Expression &&to_move): expr(std::move(to_move.expr)) {}

Expression::Expression(Expr expr): expr(std::move(expr)) {}

Print::Print(Expr expr): expr(std::move(expr)) {}

Print::Print(Print &&to_move): expr(std::move(to_move.expr)) {}

Stmt::Stmt(Expression expression): expression(std::move(expression)) {
    this->ty = StmtTy::STMT_EXPR;
}

Stmt::Stmt(Print print): print(std::move(print)) {
    this->ty = StmtTy::STMT_PRINT;
}

Stmt::Stmt(Stmt &&to_move) {
    this->ty = to_move.ty;
    switch (to_move.ty) {
        case StmtTy::STMT_EXPR:
            init_union_field(this->expression, Expression, std::move(to_move.expression));
                             break;
        case StmtTy::STMT_PRINT:
            init_union_field(this->print, Print, std::move(to_move.print));
            break;
        default:
            std::cerr << "Unknown Statement type. This should never happen" << std::endl;
    }
}

Stmt& Stmt::operator=(Stmt &&to_move) {
    switch (this->ty) {
        case StmtTy::STMT_EXPR:
            std::destroy_at(&this->expression);
            break;
        case StmtTy::STMT_PRINT: 
            std::destroy_at(&this->print);
            break;
        default:
            throw std::runtime_error("Unknown Statement type when assigning. This should never happen");
    }
    switch (to_move.ty) {
        case StmtTy::STMT_EXPR:
            init_union_field(this->expression, Expression, std::move(expression));
            break;
        case StmtTy::STMT_PRINT: 
            init_union_field(this->print, Print, std::move(print));
            break;
        default:
            throw std::runtime_error("Unknown Statement type when assigning. This should never happen");

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
        default:
            std::cerr << "Unknown Statement type. This should never happen" << std::endl;
    }

}
