#include "lox_function.hpp"
#include "interpreter.hpp"

LoxFunction::LoxFunction(const FuncStmt *decl): decl(decl) {}

int LoxFunction::arity() {
    return this->decl->params.size();
}

LoxFunction LoxFunction::copy() const {
    return LoxFunction(this->decl);
}

LoxFunction::LoxFunction(LoxFunction &&to_move) {
    this->decl = to_move.decl;
    to_move.decl = nullptr;
}


LoxElement LoxFunction::call(Interpreter *interp, std::vector<LoxElement> args) {
    auto globals = new Env(std::move(interp->globals));
    Env env = Env(globals);
    interp->execute_block(this->decl->body.statements, std::move(env));
    // now the current environment is the "globals" environment

    // FIXME: Do we need to store interp->env and re-establish that and move globals to "globals"?
    return LoxElement::nil();
}

std::string LoxFunction::to_string() const {
    std::string name = "<fn ";
    name += this->decl->name.lexeme;
    name += ">";
    return name;
}

LoxFunction::~LoxFunction() {}
