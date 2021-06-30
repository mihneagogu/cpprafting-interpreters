#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "../LexParse/expr.hpp"
#include "../LexParse/stmt.hpp"
#include "../LexParse/tokens.hpp"

enum LoxTy { LOX_NUMBER, LOX_STRING, LOX_NIL, LOX_OBJ, LOX_BOOL };

/* An actual element (loosely, object) from Lox. It can either be
 * a string, a number, or an object (thinking in terms of Lox)
 * */
class LoxElement {
private:
  LoxElement(LoxTy _nil);
  LoxElement(const LoxElement &other);

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

  LoxElement copy() const;

  bool is_truthy() const;

  std::string stringify() const;

  LoxElement(double num);
  LoxElement(std::string str);
  LoxElement(bool b);
  LoxElement(LoxElement &&to_move);
  LoxElement &operator=(LoxElement &&to_move);




  ~LoxElement();
};

/* Yes, dynamic exceptions are bad, but for this specific case it makes sense to use one.
 * In an actual real interpreter it's better to use a variant type for error handling. However,
 * in our case, a runtime error occurring is a fine case for throwing an exception, there is no
 * way of recovering from such an error, so we just halt. I.e the performance sacrifice of an exception
 * is not a problem, since it is non-recoverable as far as the user is concerned. */
class LoxRuntimeErr: std::exception {
  private:
    Token where;
    std::string why;
  public:
    std::string diagnostic() const;
    LoxRuntimeErr(Token where, std::string why);
    LoxRuntimeErr(Token where, const char *why);
};

class DivisionByZeroErr: public LoxRuntimeErr{
  public:
    DivisionByZeroErr(Token where, std::string why): LoxRuntimeErr(std::move(where), std::move(why)) {}
};

class Env {
private:
  std::unordered_map<std::string, LoxElement> values;

public:
  Env();
  void define(std::string name, LoxElement val);
  void assign(Token name, LoxElement val);
  LoxElement& get(const Token &name);
};


class Interpreter {
private:
  Env env;
  // HACK. Do we just return pointers (or shared_ptrs) to LoxElements? variable exprs have return type references to
  // LoxElements which are valid as long as they are in the map (AND are not variables which
  // have been redefined since the reference was given out)
  LoxElement evaluate_literal(const Literal &literal);
  LoxElement evaluate_binary_expr(const BinaryExpr &binary);
  LoxElement evaluate_grouping_expr(const GroupingExpr &group);
  LoxElement evaluate_unary_expr(const UnaryExpr &group);
  LoxElement& evaluate_variable_expr(const VariableExpr& var);
  LoxElement evaluate_assign_expr(const AssignExpr& assign);
  bool check_number_operand(const Token &tok, const LoxElement &right);
  bool check_bool_operand(const Token &tok, const LoxElement &right);
  bool check_number_operands(const Token &tok, const LoxElement &left, const LoxElement &right);

  void run_print_stmt(const Print &print);
  void run_expression_stmt(const Expression &expression);
  void run_var_stmt(const Var& var);
  void execute(const Stmt &stmt);


public:
  Interpreter();
  void interpret(const std::vector<Stmt> &statements);
  LoxElement evaluate(const Expr &expr);
};


#endif // INTERPRETER_H_
