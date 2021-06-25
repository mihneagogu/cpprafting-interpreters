#ifndef TOKENS_H_
#define TOKENS_H_

#include <string>
#include "../util.hpp"

enum TokenType {
    UNASSIGNED,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

// One or two character tokens.
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

// Literals.
    IDENTIFIER, STRING, NUMBER,

// Keywords.
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

    TOKENEOF
};

std::string token_type_to_string(TokenType ty);

enum LiteralTy {
    LIT_NUMBER, LIT_STRING, LIT_NIL, LIT_BOOL, LIT_UNASSIGNED
};

class Literal {
private:
    Literal(const Literal &other);

    Literal(LiteralTy nil);

public:

    LiteralTy ty;
    union {
        double number;
        std::string str;
        bool lox_bool;
        LiteralTy nil; // The literal is Lox's "nil". This is just a placeholder
    };

    Literal(double number);

    Literal(std::string str);

    Literal(bool lox_bool);

    static Literal lox_nil();

    static Literal lox_false();

    static Literal lox_true();

    Literal(Literal &&to_move);

    Literal &operator=(Literal &&to_move);

    Literal clone() const;

    ~Literal();


};

class Token {
private:
    int line;

    Token(const Token &other);

public:
    Option<Literal> literal; // TODO: Change this once we find out what it is
    TokenType type;
    std::string lexeme;

    int get_line() const;

    Token clone() const;

    Token(TokenType type, std::string lexeme, Option<Literal> literal, int line);

    Token(Token &&other);

    Token &operator=(Token &&to_move);

    ~Token() = default;

    std::string to_string() const;

};


#endif // TOKENS_H_
