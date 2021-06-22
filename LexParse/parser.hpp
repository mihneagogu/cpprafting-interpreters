#ifndef PARSER_H_
#define PARSER_H_

#include <vector>

#include "tokens.hpp"
#include "expr.hpp"

/*
   expression     → equality ;
   equality       → comparison ( ( "!=" | "==" ) comparison )* ;
   comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
   term           → factor ( ( "-" | "+" ) factor )* ;
   factor         → unary ( ( "/" | "*" ) unary )* ;
   unary          → ( "!" | "-" ) unary
                    | primary ;
   primary        → NUMBER | STRING | "true" | "false" | "nil"
                    | "(" expression ")" ;
*/

// Parser class for Lox, with the above-defined grammar 
class Parser {
    private:
        int current;

        Expr expression();
        Expr equality();
        Expr comparison();
        Expr term();
        Expr factor();
        Expr unary();
        Expr primary();

        bool match(/*TokenTy ...*/);
        bool check(TokenType ty);
        bool is_at_end();

        Token advance();
        Token peek();
        Token previous();
    public:
        std::vector<Token> tokens;
        Parser(std::vector<Token> tokens);

};


#endif // PARSER_H_
