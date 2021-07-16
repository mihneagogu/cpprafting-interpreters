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
    auto before = std::move(interp->env);
    auto globals = new Env(std::move(interp->globals));
    Env env = Env(globals);
    auto params_size = this->decl->params.size();
    for (int i = 0; i < params_size; i++) {
        env.define(this->decl->params[i].lexeme, std::move(args[i]));
    }

    interp->execute_block(this->decl->body.statements, std::move(env));
    // Now, the current environment is the "globals" environment
    // So we re-establish the environment from before
    interp->globals = std::move(interp->env);
    interp->env = std::move(before);


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
