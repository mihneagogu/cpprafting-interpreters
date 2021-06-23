#ifndef PARSER_H_
#define PARSER_H_

#include <exception>
#include <stdexcept>
#include <vector>

#include "tokens.hpp"
#include "../util.hpp"
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

        bool match(int n_types, .../*TokenTy... types*/);
        bool match(TokenType ty);
        bool check(TokenType ty);
        bool is_at_end();

        [[noreturn]] void error(Token &tok, std::string &message);
        [[noreturn]] void error(Token &tok, const char  *message);

        Token advance();
        Token& peek();
        Token& previous();
        Token consume(TokenType ty, std::string &message);

    public:
        std::vector<Token> tokens;
        Expr parse();
        Parser(std::vector<Token> tokens);

};



#endif // PARSER_H_
