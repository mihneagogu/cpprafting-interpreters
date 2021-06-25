#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#include <string>

#include "../LexParse/expr.hpp"
#include "../LexParse/tokens.hpp"

enum LoxTy { LOX_NUMBER, LOX_STRING, LOX_NIL, LOX_OBJ, LOX_BOOL };

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
  bool is_instance_of(LoxTy ty);

  LoxElement clone();

  LoxElement(double num);
  LoxElement(std::string str);
  LoxElement(bool b);
  LoxElement(LoxElement &&to_move);
  LoxElement &operator=(LoxElement &&to_move);

  static LoxElement fromLiteral(const Literal &literal);
  static LoxElement fromExpr(const Expr &expr);
  static LoxElement fromGroupingExpr(const GroupingExpr &group);
  static LoxElement fromUnaryExpr(const GroupingExpr &group);

  ~LoxElement();
};

class Interpreter {
public:
  static LoxElement evaluate(const Expr &expr);
};

#endif // INTERPRETER_H_
