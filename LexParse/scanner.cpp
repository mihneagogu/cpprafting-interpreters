#include "scanner.hpp"
#include "../main.hpp"
#include "tokens.hpp"
#include <cstring>
#include <string>
#include <cstdlib>

Scanner::Scanner(const char *content, long src_len) {
  this->src = content;
  this->src_len = src_len;
}

bool Scanner::is_at_end() const { return this->current >= this->src_len; };

bool Scanner::match(char expected) {
  if (is_at_end()) {
    return false;
  }
  if (this->src[this->current] != expected) {
    return false;
  }
  this->current++;
  return true;
}

void Scanner::string() {
  while (peek() != '"' && !is_at_end()) {
    if (peek() == '\n') {
      this->line++;
    }
    advance();
  }

  if (is_at_end()) {
    Lox::error(line, "Unterminated string");
    return;
  }

  advance(); // Match closing '"'
  // TODO: check the length is right
  int n = this->current - 1 - this->start; // String spans from start
  char copy[n+1];
  strncpy(copy, this->src + start + 1, n);
  copy[n] = '\0';
  std::string *str = new std::string(copy);
  // TODO: Change the string * to std::move(str) once we know the concrete type of literal later in the spec
  add_token(TokenType::STRING, (void *) str);
}

char Scanner::peek_next() {
  if (current + 1 >= this->src_len) { return '\0'; }
  return this->src[this->current + 1];
}

char Scanner::peek() {
  if (is_at_end()) {
    return '\0';
  }
  return this->src[this->current];
}

void Scanner::scan_token() {
  char c = advance();
  switch (c) {
      case '(':
        add_token(TokenType::LEFT_PAREN);
        break;
      case ')':
        add_token(TokenType::RIGHT_PAREN);
        break;
      case '{':
        add_token(TokenType::LEFT_BRACE);
        break;
      case '}':
        add_token(TokenType::RIGHT_PAREN);
        break;
      case ',':
        add_token(TokenType::COMMA);
        break;
      case '.':
        add_token(TokenType::DOT);
        break;
      case '-':
        add_token(TokenType::MINUS);
        break;
      case '+':
        add_token(TokenType::PLUS);
        break;
      case ';':
        add_token(TokenType::SEMICOLON);
        break;
      case '*':
        add_token(TokenType::STAR);
        break;
      case '!':
        add_token(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
      case '=':
        add_token(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
      case '<':
        add_token(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
      case '>':
        add_token(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
      case '/':
        if (match('/')) {
          while (peek() != '\n' && !is_at_end()) {
            advance();
          }
        } else {
          add_token(TokenType::SLASH);
        }
        break;
      case ' ':
      case '\r':
      case '\t':
        break;
      case '\n':
        this->line++;
        break;
    case '"':
      string();
      break;
     default:
       if (Scanner::is_digit(c)) {
         number();
       } else if (Scanner::is_alpha(c)) {
          identifier();
       } else {
        Lox::error(line, "Unexpected character");
       }
        break;
  }
}

void Scanner::identifier() {
  while(Scanner::is_alpha_numeric(peek())) { advance(); }


  std::string text = "";
  auto iter = Lox::keywords.find(text);
  TokenType ty;
  if (iter != Lox::keywords.end()) {
    ty = iter->second;
  } else {
    ty = TokenType::IDENTIFIER;
  }
  add_token(ty);
}

bool Scanner::is_alpha_numeric(char c) {
  return Scanner::is_alpha(c) || Scanner::is_digit(c);
}

bool Scanner::is_alpha(char c) {
  return (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') ||
    c == '_';
}

bool Scanner::is_digit(char c) {
  return c >= '0' && c <= '9';
}

void Scanner::number() {
  while (Scanner::is_digit(peek())) { advance(); }

  // Look for a fractional part
  if (peek() == '.' && Scanner::is_digit(peek_next())) {
    advance();
    while (is_digit(peek())) { advance(); }
  }

  int len = this->current - this->current;
  char copy[len+1];
  strncpy(copy, this->src + current, len);
  copy[len] = '\0';
  char *_unused;
  // TODO: Replace the new call to double once we know the type of literal.
  double *d = new double{strtod(copy, &_unused)};
  add_token(TokenType::NUMBER, (void *) d);
}

char Scanner::advance() { return this->src[this->current++]; }

void Scanner::add_token(TokenType type) { add_token(type, nullptr); }
void Scanner::add_token(TokenType type, void *literal) {
  int n = this->current - this->start;
  char txt[n + 1];
  strncpy(txt, this->src + start, n);
  txt[n] = '\0';
  std::string text(txt);
  this->tokens.push_back(Token{type, std::move(text), literal, this->line});
}

/* Captures [this->start .. this->current) as a string */
std::string Scanner::capture_to_string() {
  int n = this->current - this->start;
  char txt[n + 1];
  strncpy(txt, this->src + start, n);
  txt[n] = '\0';
  return std::string(txt);
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
