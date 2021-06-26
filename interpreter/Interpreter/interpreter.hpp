#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include <string>

#include "../LexParse/expr.hpp"
#include "../LexParse/tokens.hpp"

enum LoxTy { LOX_NUMBER, LOX_STRING, LOX_NIL, LOX_OBJ, LOX_BOOL };

/* Yes, dynamic exceptions are bad, but for this specific case it makes sense to use one.
 * In an actual real interpreter it's better to use a variant type for error handling. However,
 * in our case, a runtime error occurring is a fine case for throwing an exception, there is no
 * way of recovering from such an error, so we just halt. I.e the performance sacrifice of an exception
 * is not a problem, since it is non-recoverable as far as the user is concerned. */
class LoxRuntimeErr: std::exception {};

/* An actual element (loosely, object) from Lox. It can either be
 * a string, a number, or an object (thinking in terms of Lox)
 * */
class LoxElement {
private:
  LoxElement(LoxTy _nil);

public:
  LoxTy ty;
  union {
    double lox_number;
    std::string lox_str;
    bool lox_bool;
    /* LoxObject obj; */
    LoxTy lox_nil; // should only ever be LOX_NIL, it's a placeholder
  };
  static LoxElement nil();
  bool is_instance_of(LoxTy ty) const;
  bool is_number() const;
  double as_number() const;
  bool is_nil() const;

  bool equals(const LoxElement &other);

  LoxElement clone() const;

  bool is_truthy() const;

  LoxElement(double num);
  LoxElement(std::string str);
  LoxElement(bool b);
  LoxElement(LoxElement &&to_move);
  LoxElement &operator=(LoxElement &&to_move);




  ~LoxElement();
};

class Interpreter {
private:
  LoxElement evaluate_literal(const Literal &literal);
  LoxElement evaluate_binary_expr(const BinaryExpr &binary);
  LoxElement evaluate_grouping_expr(const GroupingExpr &group);
  LoxElement evaluate_unary_expr(const UnaryExpr &group);
  bool check_number_operand(const Token &tok, const LoxElement &right);
  bool check_bool_operand(const Token &tok, const LoxElement &right);
  bool check_number_operands(const Token &tok, const LoxElement& left, const LoxElement &right);


public:
  LoxElement evaluate(const Expr &expr);
};

#endif // INTERPRETER_H_
