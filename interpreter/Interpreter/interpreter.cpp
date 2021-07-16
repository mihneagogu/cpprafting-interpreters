#include "interpreter.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <chrono>
#include <sys/time.h>
#include <time.h>

#include "../util.hpp"
#include "lox_function.hpp"

LoxElement::LoxElement(LoxTy _nil) {
    ASSERT_COND(
            _nil == LoxTy::LOX_NIL,
            "LoxElement nil constructor should only be called with LOX_NIL\n");
    this->ty = LoxTy::LOX_NIL;
    this->lox_nil = this->ty;
}

LoxElement::LoxElement(double num) {
    this->ty = LoxTy::LOX_NUMBER;
    this->lox_number = num;
}

LoxElement::LoxElement(bool b) {
    this->ty = LoxTy::LOX_BOOL;
    this->lox_bool = b;
}

LoxElement::LoxElement(std::string str) {
    this->ty = LoxTy::LOX_STRING;
    init_union_field(this->lox_str, std::string, std::move(str));
}

LoxElement::LoxElement(LoxCallable *callable): callable(std::shared_ptr<LoxCallable>(callable)) {
    this->ty = LoxTy::LOX_CALLABLE;
}

LoxElement::LoxElement(LoxElement &&to_move) : ty(to_move.ty) {
    switch (to_move.ty) {
        case LoxTy::LOX_NUMBER:
            this->lox_number = to_move.lox_number;
            break;
        case LoxTy::LOX_STRING:
            init_union_field(this->lox_str, std::string, std::move(to_move.lox_str));
            break;
        case LoxTy::LOX_NIL:
            this->lox_nil = LoxTy::LOX_NIL;
            break;
        case LoxTy::LOX_BOOL:
            this->lox_number = to_move.lox_number;
            break;
        case LoxTy::LOX_CALLABLE:
            init_union_field(this->callable, std::shared_ptr<LoxCallable>, std::move(to_move.callable));
            break;
        case LoxTy::LOX_OBJ:
            break;
        default:
            std::cerr << "Unknown LoxElement type. This should never happen"
                << std::endl;
    }
}

LoxElement LoxElement::nil() { return LoxElement(LoxTy::LOX_NIL); }

LoxElement::~LoxElement() {
    switch (this->ty) {
        case LoxTy::LOX_STRING:
            std::destroy_at(&this->lox_str);
            break;
        case LoxTy::LOX_CALLABLE:
            std::destroy_at(&this->callable);
            break;
        case LoxTy::LOX_OBJ:
        case LoxTy::LOX_BOOL:
        case LoxTy::LOX_NUMBER:
        case LoxTy::LOX_NIL:
            break;
        default:
            std::cerr << "Unknown LoxElement type. This should never happen"
                << std::endl;
            break;
    }
}

LoxElement &LoxElement::operator=(LoxElement &&to_move) {
    switch (this->ty) {
        case LoxTy::LOX_STRING:
            std::destroy_at(&this->lox_str);
            break;
        case LoxTy::LOX_CALLABLE:
            std::destroy_at(&this->callable);
            break;
        case LoxTy::LOX_OBJ:
        case LoxTy::LOX_BOOL:
        case LoxTy::LOX_NUMBER:
        case LoxTy::LOX_NIL:
            break;
        default:
            std::cerr << "Unknown LoxElement type. This should never happen"
                << std::endl;
            break;
    }

    switch (to_move.ty) {
        case LoxTy::LOX_STRING:
            this->lox_str = std::move(to_move.lox_str);
            break;
        case LoxTy::LOX_OBJ:
        case LoxTy::LOX_BOOL:
            this->lox_bool = to_move.lox_bool;
            break;
        case LoxTy::LOX_NUMBER:
            this->lox_number = to_move.lox_number;
            break;
        case LoxTy::LOX_NIL:
            this->lox_nil = LoxTy::LOX_NIL;
            break;
        case LoxTy::LOX_CALLABLE:
            init_union_field(this->callable, std::shared_ptr<LoxCallable>, std::move(to_move.callable));
            break;
        default:
            std::cerr << "Unknown LoxElement type. This should never happen"
                << std::endl;
            break;
    }
    this->ty = to_move.ty;
    return *this;
}

Interpreter::Interpreter() {
    this->globals.define("clock", LoxElement(new NativeClockFn{}));
}

/*
 ** Constructs a LoxElement from the given literal. We make the distinction
 *between
 ** LiteralTy and LoxTy because it keeps the interpreter clear and the
 *interpreting stage
 */
LoxElement Interpreter::evaluate_literal(const Literal &literal) {
    switch (literal.ty) {
        case LiteralTy::LIT_BOOL:
            return LoxElement(literal.lox_bool);
        case LiteralTy::LIT_NIL:
            return LoxElement::nil();
        case LiteralTy::LIT_NUMBER:
            return LoxElement(literal.number);
        case LiteralTy::LIT_STRING:
            return LoxElement(literal.str);
        default:
            throw std::runtime_error("Unknown Literal type. This should never happen");
    }
}

LoxElement Interpreter::evaluate_unary_expr(const UnaryExpr &unary) {
    auto right = evaluate(*unary.right);
    switch (unary.op.type) {
        case TokenType::MINUS:
            check_number_operand(unary.op, right);
            return LoxElement(-right.as_number());
        case TokenType::BANG:
            check_bool_operand(unary.op, right);
            return LoxElement(!right.is_truthy());
            break;
        default:
            throw std::runtime_error(
                    "Unknown unary operator when evaluating expression.");
            break;
    }

    return right;
}

LoxElement Interpreter::evaluate_logical_expr(const LogicalExpr &logical) {
    LoxElement left = evaluate(*logical.left);
    if (logical.op.type == TokenType::OR) {
        if (left.is_truthy()) {
            return left;
        }
    } else {
        if (!left.is_truthy()) {
            return left;
        }
    }
    return evaluate(*logical.right);
}

bool LoxElement::is_number() const { return is_instance_of(LoxTy::LOX_NUMBER); }

double LoxElement::as_number() const {
    if (this->ty == LoxTy::LOX_NUMBER) {
        return this->lox_number;
    }
    throw std::runtime_error("LoxElement isn't a number!");
}

bool LoxElement::equals(const LoxElement &other) {
    if (is_nil() && other.is_nil()) {
        return true;
    } // Two nils are equal
    if (is_nil() || other.is_nil()) {
        return false;
    } // If only one of them is nil, they are not equal
    if (this->ty != other.ty) {
        return false;
    }
    switch (this->ty) {
        case LoxTy::LOX_NUMBER:
            return this->lox_number == other.lox_number;
        case LoxTy::LOX_BOOL:
            return this->lox_bool == other.lox_bool;
        case LoxTy::LOX_STRING:
            return this->lox_str == other.lox_str;
        case LoxTy::LOX_OBJ:
            // TODO
            return false;
        default:
            std::cerr << "Unknown LoxElement type. This should never happen"
                << std::endl;
            return false;
    }
}

LoxElement Interpreter::evaluate_binary_expr(const BinaryExpr &binary) {
    auto left = evaluate(*binary.left);
    auto right = evaluate(*binary.right);

    switch (binary.op.type) {
        case TokenType::MINUS:
            // Perform binop on exprs and check that they are the right type
            check_number_operands(binary.op, left, right);
            return LoxElement(left.as_number() - right.as_number());
        case TokenType::SLASH:
            // Perform binop on exprs and check that they are the right type
            check_number_operands(binary.op, left, right);
            if (right.as_number() == 0.0) {
                throw DivisionByZeroErr{binary.op.clone(), "Cannot divide by zero"};
            }
            return LoxElement(left.as_number() / right.as_number());
        case TokenType::STAR:
            // Perform binop on exprs and check that they are the right type
            check_number_operands(binary.op, left, right);
            return LoxElement(left.as_number() * right.as_number());
        case TokenType::PLUS:
            if (left.is_number() && right.is_number()) {
                return LoxElement(left.as_number() + right.as_number());
            }
            if (left.is_instance_of(LoxTy::LOX_STRING) &&
                    right.is_instance_of(LoxTy::LOX_STRING)) {
                std::string res = left.lox_str;
                res += right.lox_str;
                return LoxElement(std::move(res));
            }
            throw LoxRuntimeErr{binary.op.clone(), "Operation '+' exists only on numbers and strings"};
        case TokenType::GREATER:
            check_number_operands(binary.op, left, right);
            return LoxElement(left.as_number() > right.as_number());
        case TokenType::GREATER_EQUAL:
            check_number_operands(binary.op, left, right);
            return LoxElement(left.as_number() >= right.as_number());
        case TokenType::LESS:
            check_number_operands(binary.op, left, right);
            return LoxElement(left.as_number() < right.as_number());
        case TokenType::LESS_EQUAL:
            check_number_operands(binary.op, left, right);
            return LoxElement(left.as_number() <= right.as_number());
        case TokenType::BANG_EQUAL:
            return !left.equals(right);
        case TokenType::EQUAL_EQUAL:
            return left.equals(right);
        default:
            std::cerr << "Unknown binary operator. This should never happen"
                << std::endl;
            break;
    }
    UNREACHABLE();
}

LoxElement Interpreter::evaluate_grouping_expr(const GroupingExpr &group) {
    return evaluate(*group.expression);
}

bool Interpreter::check_number_operand(const Token &tok,
        const LoxElement &right) {
    if (right.is_number()) {
        return true;
    }
    throw LoxRuntimeErr{tok.clone(), "Operand must be a number."};
}

bool Interpreter::check_bool_operand(const Token &tok,
        const LoxElement &right) {
    if (right.ty != LoxTy::LOX_BOOL) {
        throw LoxRuntimeErr{tok.clone(), "Operand must be a boolean."};
    }
    return true;
}

bool Interpreter::check_number_operands(const Token &tok,
        const LoxElement &left,
        const LoxElement &right) {
    if (left.ty != LoxTy::LOX_NUMBER || right.ty != LoxTy::LOX_NUMBER) {
        throw LoxRuntimeErr{tok.clone(), "Operand must be a number."};
    }
    return true;
}

bool LoxElement::is_instance_of(LoxTy ty) const { return this->ty == ty; }

bool LoxElement::is_nil() const { return this->ty == LoxTy::LOX_NIL; }

bool LoxElement::is_truthy() const {
    if (is_instance_of(LoxTy::LOX_BOOL)) {
        return this->lox_bool;
    }
    if (is_nil()) {
        return false;
    }
    return true;
}

bool LoxElement::is_callable() const {
    return this->ty == LoxTy::LOX_CALLABLE;
}

std::string LoxElement::stringify() const {
    switch (this->ty) {
        case LoxTy::LOX_STRING:
            return this->lox_str;
        case LoxTy::LOX_NUMBER:
            return std::to_string(this->lox_number);
        case LoxTy::LOX_BOOL:
            return std::to_string(this->lox_bool);
        case LoxTy::LOX_NIL:
            return "nil";
        case LoxTy::LOX_CALLABLE:
            return this->callable->to_string();
        case LoxTy::LOX_OBJ:
            UNREACHABLE();
        default:
            throw std::runtime_error("Unknown LoxElement type. This should never happen");
    }
}


LoxElement &Interpreter::evaluate_variable_expr(const VariableExpr &var) {
    if (this->globals.contains(var.name)) {
       return this->globals.get(var.name);
    }
    return this->env.get(var.name);
}

LoxElement Interpreter::evaluate_assign_expr(const AssignExpr &assign) {
    auto value = evaluate(*assign.value);
    this->env.assign(assign.name.clone(), std::move(value));
    return value.copy();
}

LoxElement Interpreter::evaluate_call_expr(const CallExpr &call) {
    auto callee = evaluate(*call.callee);
    std::vector<LoxElement> args{};
    for (auto& arg : call.args) {
        args.push_back(evaluate(arg));
    }
    if (!callee.is_callable()) {
        throw LoxRuntimeErr(call.paren.clone(), "Can only call functions and classes.");
    }
    auto &callable = *callee.callable;
    int arrity;
    if (args.size() != (arrity = callable.arity())) {
        std::string err = "Expected ";
        err += arrity;
        err += " arguments but got ";
        err += args.size();
        err += '.';
        throw LoxRuntimeErr(call.paren.clone(), err);
    }
    return callable.call(this, std::move(args));
}

LoxElement Interpreter::evaluate(const Expr &expr) {
    switch (expr.ty) {
        case ExprTy::BINARY:
            return evaluate_binary_expr(expr.bin);
        case ExprTy::UNARY:
            return evaluate_unary_expr(expr.unary);
        case ExprTy::GROUPING:
            return evaluate_grouping_expr(expr.group);
        case ExprTy::LITERAL:
            return evaluate_literal(expr.lit.lit);
        case ExprTy::VAR_EXPR:
            // NOTE: is this right? Potentially we need to worry if the expr is a heap-allocated object.
            // Should we use shared_ptr for that?
            return evaluate_variable_expr(expr.var_expr).copy();
        case ExprTy::ASSIGN_EXPR:
            // NOTE: same as above
            return evaluate_assign_expr(expr.ass_expr);
        case ExprTy::LOGICAL_EXPR:
            return evaluate_logical_expr(expr.logical);
        case ExprTy::CALL_EXPR:
            return evaluate_call_expr(expr.call);
        default:
            throw std::runtime_error(
                    "Unknown expression type when interpreting. This should never happen");
    }
}



void Interpreter::run_print_stmt(const Print &print) {
    auto returned = evaluate(print.expr);
    std::cout << returned.stringify() << std::endl;
}

void Interpreter::run_expression_stmt(const Expression &expression) {
    evaluate(expression.expr);
}

void Interpreter::execute(const Stmt &stmt) {
    switch (stmt.ty) {
        case StmtTy::STMT_EXPR:
            run_expression_stmt(stmt.expression);
            break;
        case StmtTy::STMT_PRINT:
            run_print_stmt(stmt.print);
            break;
        case StmtTy::STMT_VAR:
            run_var_stmt(stmt.var);
            break;
        case StmtTy::STMT_BLOCK:
            run_block_stmt(stmt.block);
            break;
        case StmtTy::STMT_WHILE:
            run_while_stmt(stmt.while_stmt);
            break;
        case StmtTy::STMT_FUNC:
            run_func_stmt(stmt.func_stmt);
            break;
        default:
            throw std::runtime_error("Unknown Statement type when executing. This should never happen");
    }
}

void Interpreter::run_var_stmt(const Var &var) {
    if (!var.initializer.is_nil()) {
        auto val = evaluate(var.initializer);
        this->env.define(var.name.lexeme, std::move(val));
    } else {
        this->env.define(var.name.lexeme, LoxElement::nil());
    }
}

void Interpreter::execute_block(const std::vector<Stmt> &statements, Env env) {
    bool thrown = false;
    // We know we are in a block, so the enclosing of "env" is the scope above,
    // so we save it here so that we can restore the state later
    Env *before = env.enclosing;
    try {
        // Execute statements under with the new environment
        this->env = std::move(env);
        for (auto &st : statements) {
            execute(st);
        }
    }
    // Totally ugly way of making sure the environment is re-established regardless if a runtime error occurred or not
    catch (LoxRuntimeErr &ler) {
        // Make sure we don't double free on the move assignment below, because "before" is this->env.enclosing
        this->env.enclosing = nullptr;
        // Re-establish state
        this->env = std::move(*before);
        // Free the temporary pointer we allocated after we move out of it
        delete before;
        thrown = true;
        throw;
    }

    if (!thrown) {
        // Explanation is in the catch clause
        this->env.enclosing = nullptr;
        this->env = std::move(*before);
        delete before;
    }
}

void Interpreter::run_block_stmt(const Block &block) {
    // We need to be careful here to create a new Env with the current enclosing scope then
    // reset after we finish executing the block.
    Env *current_enclosing = new Env(std::move(this->env));
    Env new_env = Env(current_enclosing);
    execute_block(block.statements, std::move(new_env));
}

void Interpreter::run_if_stmt(const IfStmt &if_stmt) {
    if (evaluate(if_stmt.condition).is_truthy()) {
        execute(*if_stmt.then_branch);
    } else if (if_stmt.else_branch != nullptr) {
        execute(*if_stmt.else_branch);
    }
}

void Interpreter::run_while_stmt(const WhileStmt &while_stmt) {
    while (evaluate(while_stmt.cond).is_truthy()) {
        execute(*while_stmt.body);
    }
}

void Interpreter::run_func_stmt(const FuncStmt *func_stmt) {
    auto *lox_fun = new LoxFunction(func_stmt);
    this->env.define(func_stmt->name.lexeme, LoxElement(lox_fun));
}

void Interpreter::interpret(const std::vector <Stmt> &statements) {
    try {
        for (auto &stmt : statements) {
            execute(stmt);
        }
    } catch (LoxRuntimeErr &ler) {
        std::cout << ler.diagnostic() << std::endl;
    }
}

LoxRuntimeErr::LoxRuntimeErr(Token where, std::string why) : where(std::move(where)), why(std::move(why)) {}

LoxRuntimeErr::LoxRuntimeErr(Token where, const char *why) : where(std::move(where)), why(std::string{why}) {}

std::string LoxRuntimeErr::diagnostic() const {
    std::string res = "Error at: ";
    res += this->where.lexeme;
    res += " on line ";
    res += std::to_string(this->where.get_line());
    res += ": ";
    res += this->why;
    return res;
}

Env::Env(Env *enclosing): enclosing(enclosing) {}

Env::Env(Env &&to_move): values(std::move(to_move.values)) {
    this->enclosing = to_move.enclosing;
    to_move.enclosing = nullptr;
}

Env& Env::operator=(Env &&to_move) {
    this->values = std::move(to_move.values);
    this->enclosing = to_move.enclosing;
    to_move.enclosing = nullptr;
    return *this;
}

Env::~Env() {
    if (this->enclosing != nullptr) {
        delete enclosing;
    }
}

Env::Env() {}

void Env::define(std::string name, LoxElement val) {
    auto iter = this->values.find(name);
    if (iter != this->values.end()) {
        // We allow redefinitions of variables
        this->values.erase(iter);
    }
    this->values.insert({std::move(name), std::move(val)});
}

bool Env::contains(const Token &name) {
    return this->values.find(name.lexeme) != this->values.end();
}

size_t Env::size() const {
    return this->values.size();
}

LoxElement::LoxElement(const LoxElement &other) {
    switch (other.ty) {
        case LoxTy::LOX_NIL:
            this->lox_nil = LoxTy::LOX_NIL;
            break;
        case LoxTy::LOX_NUMBER:
            this->lox_number = other.lox_number;
            break;
        case LoxTy::LOX_BOOL:
            this->lox_bool = other.lox_bool;
            break;
        case LoxTy::LOX_STRING:
            init_union_field(this->lox_str, std::string, other.lox_str);
            break;
        case LoxTy::LOX_OBJ:
            // Probably need a find a way to alias this one. Maybe LoxObj will internally contain shared_ptr so that we avoid
            // double freeing the object when we hand out a way to copy it?
            break;
        case LoxTy::LOX_CALLABLE:
            init_union_field(this->callable, std::shared_ptr<LoxCallable>, other.callable);
            break;
        default:
            std::cerr << "Unknown Lox type when copying" << std::endl;
    }
    this->ty = other.ty;
}

LoxElement LoxElement::copy() const {
    return LoxElement(*this);
}

void Env::assign(Token name, LoxElement val) {
    if (this->values.find(name.lexeme) != this->values.end()) {
        this->values.erase(name.lexeme);
        this->values.insert({std::move(name.lexeme), std::move(val)});
        return;
    }
    if (this->enclosing != nullptr) {
        this->enclosing->assign(std::move(name), std::move(val));
        return;
    }

    std::string err = "Undefined variable '";
    err += name.lexeme;
    err += "'.";
    throw LoxRuntimeErr(std::move(name), std::move(err));
}

LoxElement &Env::get(const Token &name) {
    auto iter = this->values.find(name.lexeme);
    if (iter != this->values.end()) {
        return iter->second;
    }
    if (this->enclosing != nullptr) {
        return this->enclosing->get(name);
    }
    std::string err = "Undefined variable '";
    err += name.lexeme;
    err += "'.";
    throw LoxRuntimeErr{name.clone(), err};
}

static long long get_system_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long t = tv.tv_sec;
    t *= 1000;
    t = tv.tv_usec / 1000;
    return t;
}

LoxCallable::~LoxCallable() {}

int NativeClockFn::arity() {
    return 0;
}

NativeClockFn::~NativeClockFn() {}


LoxElement NativeClockFn::call(Interpreter *interp, std::vector<LoxElement> args) {
    return LoxElement(static_cast<double>(get_system_time()));
}

std::string NativeClockFn::to_string() const {
    return "<native fn>";
}
