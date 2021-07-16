#include <cstdarg>
#include <cstdlib>
#include <iostream>

#include "../lox.hpp"
#include "expr.hpp"
#include "parser.hpp"
#include "stmt.hpp"
#include "tokens.hpp"

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)), current(0) {}

Expr Parser::assignment() {
  auto expr = or_expr();
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

Stmt Parser::function(std::string kind) {
  std::string err = "Expect ";
  err += kind;
  err += " name.";
  Token name = consume(TokenType::IDENTIFIER, err);
  std::vector<Token> params{};
  std::string err_expect_lparen = "Expect '(' after ";
  err_expect_lparen += kind;
  err_expect_lparen += " name";
  consume(TokenType::LEFT_PAREN, err_expect_lparen);
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (params.size() >= 255) {
        error(peek(), "Can't have more than 255 parameters.");
      }
      params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match(TokenType::COMMA));
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters");
  }
  std::string expect_curly_bracket = "Expect '{' before";
  expect_curly_bracket += kind;
  expect_curly_bracket += " body";
  consume(TokenType::LEFT_BRACE, expect_curly_bracket);

  auto body = block();
  return Stmt(new FuncStmt(std::move(name), std::move(params), std::move(body)));
}

Stmt Parser::declaration() {
  if (match(TokenType::FUN)) { return function("function"); }
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

Block Parser::block() {
  std::vector<Stmt> sts{};

  while (!check(TokenType::RIGHT_BRACE) && !is_at_end()) {
    auto decl = declaration();
    sts.push_back(std::move(decl));
  }
  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return sts;
}

Stmt Parser::if_statement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  auto cond = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after 'if'.");

  auto then_branch = new Stmt(statement());
  Stmt *else_branch = nullptr;
  if (match(TokenType::ELSE)) {
    else_branch = new Stmt(statement());
  }
  return IfStmt(std::move(cond), then_branch, else_branch);
}

Stmt Parser::while_statement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  auto cond = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  auto *body = new Stmt(statement());
  return Stmt(WhileStmt(std::move(cond), body));
}

Stmt Parser::for_statement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
  Stmt *initializer = nullptr;
  if (match(TokenType::SEMICOLON)) {
    // nothing
  } else if (match(TokenType::VAR)) {
    initializer = new Stmt(var_declaration());
  } else {
    initializer = new Stmt(expression_statement());
  }
  Expr *cond = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    cond = new Expr(expression());
  }
  consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

  Expr *increment = nullptr;
  if (!check(TokenType::RIGHT_PAREN)) {
    increment = new Expr(expression());
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
  auto body = statement();
  if (increment != nullptr) {
    auto inside = std::vector<Stmt>{};
    inside.push_back(std::move(body));
    inside.push_back(Stmt(Expression(std::move(*increment))));
    auto new_body = Stmt(Block(std::move(inside)));
    delete increment;
    body = std::move(new_body);
  }
  if (cond == nullptr) {
    cond = new Expr(Literal::lox_true());
  }
  auto new_body = Stmt(WhileStmt(std::move(*cond), new Stmt(std::move(body))));
  delete cond;
  body = std::move(new_body);

  if (initializer != nullptr) {
    auto inside = std::vector<Stmt>{};
    inside.push_back(std::move(*initializer));
    inside.push_back(std::move(body));
    new_body = Stmt(Block(std::move(inside)));
    delete initializer;
    body = std::move(new_body);
  }

  return body;

}

Stmt Parser::statement() {
  if (match(TokenType::FOR)) {
    return for_statement();
  }
  if (match(TokenType::IF)) {
    return if_statement();
  }
  if (match(TokenType::PRINT)) {
    return print_statement();
  }
  if (match(TokenType::WHILE)) {
    return while_statement();
  }
  if (match(TokenType::LEFT_BRACE)) {
    return Stmt(block());
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
    TokenType ty = (TokenType)va_arg(args, int);
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

Token &Parser::peek() { return this->tokens[this->current]; }

Token &Parser::previous() { return this->tokens[this->current - 1]; }

Expr Parser::comparison() {
  auto expr = Expr(term());

  while (match(4, TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
               TokenType::LESS_EQUAL)) {
    auto op = previous().clone();
    auto *right = new Expr(term());
    auto other =
        Expr(BinaryExpr(new Expr(std::move(expr)), std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::and_expr() {
  auto expr = equality();
  while (match(TokenType::AND)) {
    auto op = previous().clone();
    auto *right = new Expr(equality());
    auto other = Expr(LogicalExpr(std::move(op), new Expr(std::move(expr)), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::or_expr() {
  auto expr = and_expr();
  while (match(TokenType::OR)) {
    auto op = previous().clone();
    auto *right = new Expr(and_expr());
    auto other = Expr(LogicalExpr(std::move(op), new Expr(std::move(expr)), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::term() {
  auto expr = Expr(factor());

  while (match(2, TokenType::MINUS, TokenType::PLUS)) {
    auto op = Token(previous().clone());
    auto *right = new Expr(factor());
    auto other =
        Expr(BinaryExpr(new Expr(std::move(expr)), std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

Expr Parser::factor() {
  auto expr = Expr(unary());

  while (match(2, TokenType::SLASH, TokenType::STAR)) {
    auto op = previous().clone();
    auto *right = new Expr(unary());
    Expr other =
        Expr(BinaryExpr(new Expr(std::move(expr)), std::move(op), right));
    expr = std::move(other);
  }
  return expr;
}

constexpr size_t MAX_FUNC_ARGS = 255;

Expr Parser::finish_call(Expr *callee) {
  std::vector<Expr> args{};
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (args.size() >= MAX_FUNC_ARGS) {
        error(peek(), "Can't have more than 255 arguments.");
      }
      args.push_back(expression());
    } while (match(TokenType::COMMA));
  }
  auto paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
  return Expr(CallExpr(callee, std::move(paren), std::move(args)));
}

Expr Parser::call() {
  auto expr = primary();
  while (true) {
    if (match(TokenType::LEFT_PAREN)) {
      auto new_expr = finish_call(new Expr(std::move(expr)));
      expr = std::move(new_expr);
    } else {
      break;
    }
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
  return call();
}

constexpr int PARSE_ERR_EXIT = 63;
[[noreturn]] void Parser::error(Token &tok, const char *message) {
  auto err = std::string(message);
  Lox::error(tok, err);
  exit(PARSE_ERR_EXIT);
}

Expr Parser::primary() {
  if (match(TokenType::FALSE)) {
    return Expr(LiteralExpr(Literal::lox_false()));
  }
  if (match(TokenType::TRUE)) {
    return Expr(LiteralExpr(Literal::lox_true()));
  }
  if (match(TokenType::NIL)) {
    return Expr(LiteralExpr(Literal::lox_nil()));
  }

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

/* We are going to make a *terrific* thing for user experience and exit here and
 *now if we encounter a parsing error, doing absolutely nothing to destruct the
 *objects we've created.
 * TODO: signal upwards that we have encountered an error instead of terminating
 *the program */
void Parser::error(Token &tok, std::string &message) {
  Lox::error(tok, message);
  exit(PARSE_ERR_EXIT);
}

Token Parser::consume(TokenType ty, std::string &message) {
  if (check(ty)) {
    return advance();
  }
  error(peek(), message);
}

Token Parser::consume(TokenType ty, const char *message) {
  std::string msg(message);
  return consume(ty, msg);
}
