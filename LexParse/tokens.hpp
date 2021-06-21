#ifndef TOKENS_H_
#define TOKENS_H_

#include <string>
#include "../util.hpp"

enum TokenType {
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

enum LiteralTy { LIT_NUMBER, LIT_STRING };
class Literal {
    public:
        LiteralTy ty;
        union {
            double number;
            std::string str;
        };
        Literal(double number);
        Literal(std::string str);
        Literal(Literal &&to_move);
        Literal& operator=(Literal &&to_move);

        ~Literal();


};

class Token {
    private:
        TokenType type;
        Option<Literal> literal; // TODO: Change this once we find out what it is
        int line;

    public:
        std::string lexeme;
        Token(TokenType type, std::string lexeme, std::optional<Literal> literal, int line);

        Token(Token &&other);
        Token& operator=(Token &&to_move);
        ~Token() = default;

        std::string to_string() const;

};


#endif // TOKENS_H_
