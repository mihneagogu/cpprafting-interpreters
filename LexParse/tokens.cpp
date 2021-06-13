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

Token::~Token() {
  // TODO: Change this once we know what type literal is
  switch (this->type) {
    case TokenType::STRING:
      delete (std::string *) this->literal;
      break;
    case TokenType::NUMBER:
      delete (double *) this->literal;
      break;
    default:
      break;
  }
}

std::string Token::to_string() const {
  std::string res(token_type_to_string(this->type));
  res += " ";
  if (this->type != TokenType::TOKENEOF) {
    res += this->lexeme;
    res += " ";
  }
  if (this->literal != nullptr) {
    switch (this->type) {
      case TokenType::STRING: {
        std::string *str = (std::string *) this->literal;
        res += *str;
        break;
      }
    default:
      res += "some literal";
      break;
    }
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
