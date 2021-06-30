#ifndef PARSER_H_
#define PARSER_H_

#include <exception>
#include <stdexcept>
#include <vector>

#include "tokens.hpp"
#include "stmt.hpp"
#include "../util.hpp"
#include "expr.hpp"

/*
   program        → declaration* EOF;
   declaration    → varDecl | statement;
   varDecl        → "var" IDENTIFIER  ("=" expression )? ";" ;
   statement      → exprStmt | ifStmt | printStmt | block;
   block          → "{" declaration* "}";
   ifStmt         → "if" "(" expression ")" statement ("else" statement)? ;
   exprStmt       → expression ";";
   printStmt      → "print" expression ";";
   expression     → assignment ;
   assignment     → IDENTIFIER "=" assignment | equality | logic_or ;
   equality       → comparison ( ( "!=" | "==" ) comparison )* ;
   logic_or       → logic_and ("or" logic_and)* ;
   logic_and      → equality ("and" equality)* ;
   comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
   term           → factor ( ( "-" | "+" ) factor )* ;
   factor         → unary ( ( "/" | "*" ) unary )* ;
   unary          → ( "!" | "-" ) unary
                    | primary ;
   primary        → NUMBER | STRING | "true" | "false" | "nil"
                    | "(" expression ")" | IDENTIFIER ;
*/


// Parser class for Lox, with the above-defined grammar 
class Parser {
    private:
        int current;

        Expr assignment();
        Expr expression();
        Expr equality();
        Expr comparison();
        Expr term();
        Expr factor();
        Expr unary();
        Expr primary();
        Expr or_expr();
        Expr and_expr();

        Stmt statement();
        Stmt print_statement();
        Stmt expression_statement();
        Stmt declaration();
        Stmt var_declaration();
        Stmt if_statement();
        Block block();


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
        Token consume(TokenType ty, const char *message);

    public:
        std::vector<Token> tokens;
        std::vector<Stmt> parse();
        Parser(std::vector<Token> tokens);

};



#endif // PARSER_H_
