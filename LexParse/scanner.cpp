#include <cstring>
#include <string>
#include "scanner.hpp"
#include "tokens.hpp"
#include "../main.hpp"

Scanner::Scanner(const char *content, long src_len) {
  this->src = content;
  this->src_len = src_len;
}

bool Scanner::is_at_end() const { return this->current >= this->src_len; };

void Scanner::scan_token() {
    char c = advance();
    switch (c) {
        case '(': add_token(TokenType::LEFT_PAREN); break;
        case ')': add_token(TokenType::RIGHT_PAREN); break;
        case '{': add_token(TokenType::LEFT_BRACE); break;
        case '}': add_token(TokenType::RIGHT_PAREN); break;
        case ',': add_token(TokenType::COMMA); break;
        case '.': add_token(TokenType::DOT); break;
        case '-': add_token(TokenType::MINUS); break;
        case '+': add_token(TokenType::PLUS); break;
        case ';': add_token(TokenType::SEMICOLON); break;
        case '*': add_token(TokenType::STAR); break;
        default: std::string unexp = "Unexpected character"; Lox::error(line, unexp); break;
    }
}

char Scanner::advance() {
    return this->src[this->current++];
}

void Scanner::add_token(TokenType type) {
    add_token(type, nullptr);
}
void Scanner::add_token(TokenType type, void *literal) {
    int n = this->current - this->start;
    char txt[n + 1];
    strncpy(txt, this->src + start, n);
    txt[n] = '\0';
    std::string text(txt);
    this->tokens.push_back(Token{type, std::move(text), literal, this->line});
}

std::vector<Token> Scanner::scan_tokens() {
  while (!is_at_end()) {
    this->start = this->current;
    scan_token();
  }

  this->tokens.push_back(Token{TokenType::TOKENEOF, "", nullptr, line});
  std::vector<Token> res(std::move(this->tokens));
  return res;
}
