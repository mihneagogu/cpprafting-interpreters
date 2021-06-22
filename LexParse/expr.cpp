#include <cstdarg>
#include <stdexcept>
#include <iostream>
#include <memory>

#include "expr.hpp"
#include "../util.hpp"

BinaryExpr::BinaryExpr(BinaryExpr &&to_move): op(std::move(to_move.op)) {
    this->left = to_move.left;
    to_move.left = nullptr;
    this->right = to_move.right;
    to_move.right = nullptr;
}

BinaryExpr::BinaryExpr(Expr *left, Token op, Expr *right): op(std::move(op)) {
    this->left = left;
    this->right = right;
}

std::string BinaryExpr::parenthesize() const {
    return Expr::parenthesize(this->op.lexeme, 2, this->left, this->right);
}

BinaryExpr::~BinaryExpr() {
    if (this->left != nullptr) {
        delete this->left;
    }
    if (this->right != nullptr) {
        delete this->right;
    }
}

GroupingExpr::GroupingExpr(Expr *expr): expression(expr) {}

GroupingExpr::GroupingExpr(GroupingExpr &&to_move) {
    this->expression = to_move.expression;
    to_move.expression = nullptr;
}


std::string GroupingExpr::parenthesize() const {
    return Expr::parenthesize("group", 1, this->expression);
}

GroupingExpr::~GroupingExpr() {
    if (this->expression != nullptr) {
        delete this->expression;
    }
}

LiteralExpr::LiteralExpr(Option<Literal> maybe_lit) {
    this->maybe_lit = std::move(maybe_lit);
}

LiteralExpr::LiteralExpr(LiteralExpr &&to_move): maybe_lit(std::move(to_move.maybe_lit)) {}

std::string LiteralExpr::parenthesize() const {
    if (!this->maybe_lit.has_value()) {
        return "nil";
    }
    switch (this->maybe_lit->ty) {
        case LiteralTy::LIT_NUMBER:
            return std::to_string(this->maybe_lit->number);
        case LiteralTy::LIT_STRING:
            return this->maybe_lit->str;
        default:
            return "some_literal";
    }
}

LiteralExpr::~LiteralExpr() = default;

UnaryExpr::UnaryExpr(Token op, Expr *right): op(std::move(op)) {
    this->right = right;
}

UnaryExpr::UnaryExpr(UnaryExpr &&to_move): op(std::move(to_move.op)) {
    this->right = to_move.right;
    to_move.right = nullptr;
}


std::string UnaryExpr::parenthesize() const {
    return Expr::parenthesize(this->op.lexeme, 1, this->right);
}

UnaryExpr::~UnaryExpr() {
    if (this->right != nullptr) {
        delete this->right;
    }
}


Expr::Expr(BinaryExpr bin) {
    this->ty = ExprTy::BINARY;
    init_union_field(this->bin, BinaryExpr, std::move(bin));
}

Expr::Expr(GroupingExpr group) {
    this->ty = ExprTy::GROUPING;
    init_union_field(this->group, GroupingExpr, std::move(group));
}

Expr::Expr(LiteralExpr lit) {
    this->ty = ExprTy::LITERAL;
    init_union_field(this->lit, LiteralExpr, std::move(lit));
}

Expr::Expr(UnaryExpr unary) {
    this->ty = ExprTy::UNARY;
    init_union_field(this->unary, UnaryExpr, std::move(unary));
}

Expr::Expr(Expr&& to_move) {
    this->ty = to_move.ty;
    switch (this->ty) {
        case ExprTy::BINARY:
            init_union_field(this->bin, BinaryExpr, std::move(to_move.bin));
            break;
        case ExprTy::GROUPING:
            init_union_field(this->group, GroupingExpr, std::move(to_move.group));
            break;
        case ExprTy::LITERAL:
            init_union_field(this->lit, LiteralExpr, std::move(to_move.lit));
            break;
        case ExprTy::UNARY:
            init_union_field(this->unary, UnaryExpr, std::move(to_move.unary));
            break;
        default:
            std::cerr << "Unknown Literal type when constructing an Expr. This should never happen" << std::endl;
    }

}

Expr::~Expr() {
    switch (this->ty) {
        case ExprTy::BINARY:
            std::destroy_at(&this->bin);
            break;
        case ExprTy::GROUPING:
            std::destroy_at(&this->group);
            break;
        case ExprTy::LITERAL:
            std::destroy_at(&this->lit);
            break;
        case ExprTy::UNARY:
            std::destroy_at(&this->unary);
            break;
        default:
            std::cerr << "Unknown Expression type. This should never happen" << std::endl;
            break;
    }

}


std::string Expr::parenthesize() const {
    switch (this->ty) {
        case ExprTy::BINARY:
            return this->bin.parenthesize();
        case ExprTy::GROUPING:
            return this->group.parenthesize();
        case ExprTy::LITERAL:
            return this->lit.parenthesize();
        case ExprTy::UNARY:
            return this->unary.parenthesize();
        default:
            throw std::runtime_error("Unknown expression type. This should never happen");
    }
}

std::string Expr::parenthesize(const std::string& name, int n_args, ...) {

    va_list args;
    va_start(args, n_args);

    std::string res = "";
    res += '(';
    res += name;

    for (int i = 0; i < n_args; i++) {
        const Expr *e = va_arg(args, const Expr *);
        res += ' ';
        res += e->parenthesize();
    }
    res += ')';
    return res;
}
