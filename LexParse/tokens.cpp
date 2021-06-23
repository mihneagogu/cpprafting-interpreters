#include <stdexcept>
#include <string>
#include <memory>
#include <iostream>
#include "tokens.hpp"
#include "../util.hpp"

Literal::~Literal() {
  switch (this->ty) {
    case LiteralTy::LIT_STRING:
      std::destroy_at(&this->str);
      break;
    case LiteralTy::LIT_NUMBER:
      break;
    default:
      std::cerr << "Unknown Literal type when destructing Literal. This should never happen" << std::endl;
  }
}

Literal::Literal(bool lox_value) {
  this->ty = LiteralTy::LOX_BOOL;
  this->lox_bool = lox_value;
}

Literal Literal::lox_false() {
  return Literal(false);
}

Literal Literal::lox_true() {
  return Literal(true);
}

Literal::Literal(LiteralTy _nil) {
  ASSERT_COND(_nil == LiteralTy::LIT_NIL, "Nil literal constructor can only be passed LIT_NIL\n");
  this->ty = LiteralTy::LIT_NIL;
  this->nil = this->ty;
}

Literal Literal::lox_nil() {
  return Literal(LiteralTy::LIT_NIL);
}

Literal::Literal(const Literal& other) {
  this->ty = other.ty;
  switch (this->ty) {
    case LiteralTy::LIT_NUMBER:
      this->number = other.number;
      break;
    case LiteralTy::LIT_STRING:
      init_union_field(this->str, std::string, other.str);
      break;
    case LiteralTy::LIT_NIL:
      this->nil = LiteralTy::LIT_NIL;
      break;
    default:
      std::cerr << "Unknown Literal type. This should never happen" << std::endl;
  }
}

Literal::Literal(double number) {
  this->ty = LiteralTy::LIT_NUMBER;
  this->number = number;
}

Literal::Literal(std::string str): str(std::move(str)) {
  this->ty = LiteralTy::LIT_STRING;
}


Literal& Literal::operator=(Literal &&to_move) {
  this->ty = to_move.ty;
  switch (to_move.ty) {
    case LiteralTy::LIT_NUMBER:
      this->number = to_move.number;
      break;
    case LiteralTy::LIT_STRING:
      // Carefully construct str properly, without reading uninited memory
      // (because we are using a union)
      init_union_field(this->str, std::string, std::move(to_move.str));
      break;
    default:
      throw std::runtime_error("Unknown Literal type. This should never happen");
  }
  return *this;
}

Literal Literal::clone() const {
  return Literal(*this);
}

Literal::Literal(Literal &&to_move) {
  this->ty = to_move.ty;
  switch (to_move.ty) {
    case LiteralTy::LIT_NUMBER:
      this->number = to_move.number;
      break;
    case LiteralTy::LIT_STRING:
      // Carefully construct str properly, without reading uninited memory
      // (because we are using a union)
      init_union_field(this->str, std::string, std::move(to_move.str));
      break;
    default:
      // throw std::runtime_error("Unknown Literal type. This should never happen");
      return;
  }

}


Token::Token(TokenType type, std::string lexeme, Option<Literal> lit, int line): literal(std::move(lit)), lexeme(std::move(lexeme)) {
  this->type = type;
  this->line = line;
}

Token::Token(Token &&other): literal(std::move(other.literal)), lexeme(std::move(other.lexeme)) {
  this->type = other.type;
  this->line = other.line;
}

Token& Token::operator=(Token &&to_move) {
  this->lexeme = std::move(to_move.lexeme);
  this->literal = std::move(to_move.literal);
  this->type = to_move.type;
  this->line = to_move.line;
  return *this;
}

Token::Token(const Token& other) {
  this->lexeme = other.lexeme;
  this->line = other.line;
  this->type = other.type;
  if (!other.literal.has_value()) {
    this->literal = None;
  } else {
    this->literal = other.literal->clone();
  }
}

Token Token::clone() const {
  return Token(*this);
}

int Token::get_line() const {
  return this->line;
}


std::string Token::to_string() const {
  std::string res(token_type_to_string(this->type));
  res += " ";
  if (this->type != TokenType::TOKENEOF) {
    res += this->lexeme;
    res += " ";
  }

  if (!this->literal.has_value()) {
    return res;
  }
  switch (this->type) {
    case TokenType::STRING:
      res += this->literal->str;
      break;
    case TokenType::NUMBER:
      res += std::to_string(this->literal->number);
      break;
    case TokenType::NIL:
      res += "nil";
      break;
    default:
      res += "some literal";
      break;
  }
  return res;
}

std::string token_type_to_string(TokenType ty) {
  switch (ty) {
  case TokenType::LEFT_PAREN:
    return "(";
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::TOKENEOF:
    return "EOF";
  case TokenType::FOR:
    return "FOR";
  default:
    return "SOME TOKEN";
  }
}
