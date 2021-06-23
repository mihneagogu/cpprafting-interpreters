#include "lox.hpp"

#include <iostream>


// YES!!! I KNOW I COULD HAVE USED CONSTEXPR FOR THIS
std::unordered_map<std::string, TokenType> Lox::keywords = {
    {"and", TokenType::AND},     {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},   {"false", TokenType::FALSE},
    {"for", TokenType::FOR},     {"if", TokenType::IF},
    {"nil", TokenType::NIL},     {"or", TokenType::OR},
    {"print", TokenType::PRINT}, {"return", TokenType::RETURN},
    {"super", TokenType::SUPER}, {"this", TokenType::THIS},
    {"true", TokenType::TRUE},   {"var", TokenType::VAR},
    {"while", TokenType::WHILE}};

// -- class Lox implementation //

bool Lox::has_err = false;
void Lox::error(int line, std::string &message) {
  std::string empty = "";
  Lox::report(line, empty, message);
}

void Lox::error(Token &tok, const std::string &message) {
  if (tok.type == TokenType::TOKENEOF) {
    std::string at = "at end";
    Lox::report(tok.get_line(), at, message);
  } else {
    std::string at = std::to_string(tok.get_line());
    at += '\'';
    at += tok.lexeme;
    at += '\'';
    Lox::report(tok.get_line(), at, message);
  }
}

void Lox::error(int line, const char *message) {
  std::string empty = "";
  Lox::report(line, empty, message);
}

void Lox::report(int line, const std::string &where, const char *message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  Lox::has_err = true;
}

void Lox::report(int line, const std::string &where, const std::string &message) {
  std::cout << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  Lox::has_err = true;
}

bool Lox::hasError() { return Lox::has_err; }
