#include "interpreter.hpp"

#include <iostream>
#include <memory>

#include "../util.hpp"

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
  default:
    std::cerr << "Unknown LoxElement type. This should never happen"
              << std::endl;
    break;
  }
  this->ty = to_move.ty;
  return *this;
}

bool LoxElement::is_instance_of(LoxTy ty) { return this->ty == ty; }
