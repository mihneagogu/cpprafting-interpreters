#include <cstdarg>
#include <cstdlib>
#include <iostream>

#include "expr.hpp"
#include "parser.hpp"
#include "tokens.hpp"
#include "stmt.hpp"
#include "../lox.hpp"

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)), current(0) {}

Expr Parser::assignment() {
  auto expr = equality();
  if (match(TokenType::EQUAL)) {
    auto equals = previous().clone();
    auto *value = new Expr(assignment());

    if (expr.ty == ExprTy::VAR_EXPR) {
      Token name = expr.var_expr.name.clone();
      return Expr(AssignExpr(std::move(name), value));
    }

    error(equals, "Invalid assignment target");
  }
  return expr;
}

Expr Parser::expression() { return assignment(); }
std::vector<Stmt> Parser::parse() {
  std::vector<Stmt> prog;
  while (!is_at_end()) {
    prog.push_back(declaration());
  }
  return prog;
}

Stmt Parser::declaration() {
  if (match(TokenType::VAR)) {
    return var_declaration();
  }
  return statement();
}

Stmt Parser::var_declaration() {
  std::string err_msg = "Expect a variable name";
  auto name = consume(TokenType::IDENTIFIER, err_msg);
  if (match(TokenType::EQUAL)) {
    auto initializer = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
    return Stmt(Var(std::move(name), std::move(initializer)));
  } else {
    auto initializer = Expr(LiteralExpr(Literal::lox_nil()));
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
    return Stmt(Var(std::move(name), std::move(initializer)));
  }
}



Stmt Parser::print_statement() {
  auto value = expression();
  std::string err_msg = "Expected ; after value.";
  consume(TokenType::SEMICOLON, err_msg);
  return Print(std::move(value));
}

Stmt Parser::expression_statement() {
  auto expr = expression();
  std::string err_msg = "Expected ; after expression.";
  consume(TokenType::SEMICOLON, err_msg);
  return Expression(std::move(expr));
}


Stmt Parser::statement() {
  if (match(TokenType::PRINT)) {
    return print_statement();
  }
  return expression_statement();
}

Expr Parser::equality() {
  auto expr = comparison();

  while (match(2, TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
    auto op = previous().clone();
    auto *right = new Expr(comparison());
    auto *left = new Expr(std::move(expr));
    Expr other = Expr(BinaryExpr(left, std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

bool Parser::match(TokenType ty) {
  if (check(ty)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::match(int n_types, /*TokenType... types*/...) {
  va_list args;
  va_start(args, n_types);
  for (int i = 0; i < n_types; i++) {
    TokenType ty = (TokenType) va_arg(args, int);
    if (check(ty)) {
      auto prev = advance();
      return true;
    }
  }
  return false;
}

bool Parser::check(TokenType ty) {
  if (is_at_end()) {
    return false;
  }
  return peek().type == ty;
}

Token Parser::advance() {
  if (!is_at_end()) {
    current++;
  }
  auto &prev = previous();
  return prev.clone();
}

bool Parser::is_at_end() { return peek().type == TokenType::TOKENEOF; }

Token& Parser::peek() { return this->tokens[this->current]; }

Token& Parser::previous() {
    return this->tokens[this->current - 1];
}

Expr Parser::comparison() {
  auto expr = Expr(term());

  while (match(4, TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
               TokenType::LESS_EQUAL)) {
    auto op = previous().clone();
    auto *right = new Expr(term());
    auto other = Expr(BinaryExpr(new Expr(std::move(expr)), std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::term() {
  auto expr = Expr(factor());

  while (match(2, TokenType::MINUS, TokenType::PLUS)) {
    auto op = Token(previous().clone());
    auto *right = new Expr(factor());
    auto other = Expr(BinaryExpr(new Expr(std::move(expr)), std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::factor() {
  auto expr = Expr(unary());

  while (match(2, TokenType::SLASH, TokenType::STAR)) {
    auto op = previous().clone();
    auto *right = new Expr(unary());
    Expr other = Expr(BinaryExpr(new Expr(std::move(expr)), std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::unary() {
  if (match(2, TokenType::BANG, TokenType::MINUS)) {
      std::cout << previous().to_string() << std::endl;
    auto op = previous().clone();
    auto *right = new Expr(unary());
    return Expr(UnaryExpr(std::move(op), right));
  }
  return primary();
}

constexpr int PARSE_ERR_EXIT = 63;
[[noreturn]] void Parser::error(Token &tok, const char *message) {
  auto err = std::string(message);
  Lox::error(tok, err);
  exit(PARSE_ERR_EXIT);

}

Expr Parser::primary() {
  if (match(TokenType::FALSE)) { return Expr(LiteralExpr(Literal::lox_false())); }
  if (match(TokenType::TRUE)) { return Expr(LiteralExpr(Literal::lox_true())); }
  if (match(TokenType::NIL)) { return Expr(LiteralExpr(Literal::lox_nil())); }

  if (match(2, TokenType::NUMBER, TokenType::STRING)) {
      auto &prev = previous();
      if (prev.literal.has_value()) {
          Literal lit = prev.literal->clone();
          return Expr(LiteralExpr(std::move(lit)));
      } else {
          return Expr(Literal::lox_nil());
      }
  }

  if (match(TokenType::IDENTIFIER)) {
      auto &prev = previous();
      auto cl = prev.clone();
    return Expr(VariableExpr(previous().clone()));
  }

  if (match(TokenType::LEFT_PAREN)) {
    auto *expr = new Expr(expression());
    std::string err = "Expect ')' after expression.";
    consume(TokenType::RIGHT_PAREN, err);
    return Expr(GroupingExpr(expr));
  }
  auto next = peek().clone();
  error(next, "Expect expression");
}

/* We are going to make a *terrific* thing for user experience and exit here and now
 *if we encounter a parsing error, doing absolutely nothing to destruct the objects we've created.
 * TODO: signal upwards that we have encountered an error instead of terminating the program */
void Parser::error(Token &tok, std::string &message) {
  Lox::error(tok, message);
  exit(PARSE_ERR_EXIT);
}


Token Parser::consume(TokenType ty, std::string &message) {
  if (check(ty)) { return advance(); }
  error(peek(), message);
}

Token Parser::consume(TokenType ty, const char *message) {
  std::string msg(message);
  return consume(ty, msg);
}
