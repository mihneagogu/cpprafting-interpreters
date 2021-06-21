#include <cstdarg>
#include <stdexcept>

#include "expr.hpp"

std::string BinaryExpr::parenthesize() const {
    return Expr::parenthesize(this->op.lexeme, 2, this->left, this->right);
}

std::string GroupingExpr::parenthesize() const {
    return Expr::parenthesize("group", 1, this->expression);
}

std::string LiteralExpr::parenthesize() const {
    if (this->maybe_value == nullptr) {
        return "nil";
    }
    // TODO(mike): Change this to actual literal when we know what the void * is
    return "some_literal";
}

std::string UnaryExpr::parenthesize() const {
    return Expr::parenthesize(this->op.lexeme, 1, this->right);
}

std::string Expr::parenthesize() const {
    switch (this->ty) {
        case ExprTy::BINARY:
            return this->content.bin.parenthesize();
        case ExprTy::GROUPING:
            return this->content.group.parenthesize();
        case ExprTy::LITERAL:
            return this->content.literal.parenthesize();
        case ExprTy::UNARY:
            return this->content.unary.parenthesize();
        default:
            throw std::runtime_error("Unknown expression type. This should never happen");
    }
}

std::string Expr::parenthesize(std::string name, int n_args, ...) {

    va_list args;
    va_start(args, n_args);

    std::string res = "";
    res += '(';
    res += std::move(name);

    for (int i = 0; i < n_args; i++) {
        const Expr *e = va_arg(args, const Expr *);
        res += '(';
        res += std::move(e->parenthesize());
    }
    res += ')';
    return res;
}
