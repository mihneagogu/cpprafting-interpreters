#include "lox_function.hpp"
#include "interpreter.hpp"

LoxFunction::LoxFunction(const FuncStmt *decl) : decl(decl) {}

int LoxFunction::arity() { return this->decl->params.size(); }

LoxFunction LoxFunction::copy() const { return LoxFunction(this->decl); }

LoxFunction::LoxFunction(LoxFunction &&to_move) {
  this->decl = to_move.decl;
  to_move.decl = nullptr;
}

LoxElement LoxFunction::call(Interpreter *interp,
                             std::vector<LoxElement> args) {
  auto before = std::move(interp->env);
  Env env{};
  auto params_size = this->decl->params.size();
  for (int i = 0; i < params_size; i++) {
    env.define(this->decl->params[i].lexeme, std::move(args[i]));
  }

  // Enable recursion
  LoxCallable *this_fn = new LoxFunction(this->decl);
  env.define(this->decl->name.lexeme, LoxElement(this_fn));

  try {
    interp->execute_block(this->decl->body.statements, std::move(env));
    // Now, the current environment is the "env.enclosing" which is NULL
    // So we re-establish the environment from before
  } catch (ReturnException *ret) {
    auto returned = std::move(ret->value);
    delete ret;
    interp->env = std::move(before);
    return returned;
  }
  interp->env = std::move(before);

  // FIXME: Do we need to store interp->env and re-establish that and move
  // globals to "globals"?
  return LoxElement::nil();
}

std::string LoxFunction::to_string() const {
  std::string name = "<fn ";
  name += this->decl->name.lexeme;
  name += ">";
  return name;
}

LoxFunction::~LoxFunction() {}
