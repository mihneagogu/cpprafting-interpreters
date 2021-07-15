#ifndef LOX_FUNCTION_H_
#define LOX_FUNCTION_H_

#include <string>

#include "interpreter.hpp"
#include "../LexParse/stmt.hpp"

class LoxFunction : public LoxCallable {
private:
  const FuncStmt *decl;
public:
  LoxFunction(const FuncStmt *decl);
  int arity();
  LoxElement call(Interpreter *interp, std::vector<LoxElement> args);
  std::string to_string() const;
  LoxFunction(LoxFunction &&to_move);
  LoxFunction copy() const;
  ~LoxFunction();
};

#endif // LOX_FUNCTION_H_
