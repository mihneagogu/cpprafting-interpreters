#ifndef TOKENS_H_
#define TOKENS_H_
#include <string>

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

class Token {
    private:
        TokenType type;
        std::string lexeme;
        void *literal; // TODO: Change this once we find out what it is
        int line;

    public:
        Token(TokenType type, std::string lexeme, void *literal, int line);

        Token(Token &&other);
        ~Token();

        std::string to_string() const;

};


#endif // TOKENS_H_
