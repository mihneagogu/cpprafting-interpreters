#include "tokens.hpp"

Token::Token(TokenType type, std::string lexeme, void *literal, int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = literal;
  this->line = line;
}

Token::Token(Token &&other) {
  // There are more concise ways of writing this, but this is really clean so it
  // will do just fine
  this->type = other.type;
  this->lexeme = std::move(other.lexeme);
  this->literal = other.literal;
  other.literal = nullptr;
  this->line = other.line;
}

std::string Token::to_string() const {
    std::string res(token_type_to_string(this->type));
    res += " ";
    res += this->lexeme;
    res += " ";
    res += "some literal";

    return res;
}


std::string token_type_to_string(TokenType ty) {
    switch (ty) {
        case LEFT_PAREN: return "(";
        default: return "SOME TOKEN";
    }
}
