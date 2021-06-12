#ifndef SCANNER_H_
#define SCANNER_H_
#include <vector>
#include "tokens.hpp"

class Scanner {
    private:
        const char *src;
        long src_len;
        int line = 1;
        int start = 0;
        int current = 0;
        std::vector<Token> tokens;

        char advance();
        void add_token(TokenType type);
        void add_token(TokenType type, void *literal);
        bool match(char expected);
        char peek();
        char peek_next();
        void string();
        static bool is_digit(char c);
        static bool is_alpha(char c);
        static bool is_alpha_numeric(char c);
        void identifier();
        void number();

        /* Captures [this->start .. this->current) as a string */
        std::string capture_to_string();

    public:
        Scanner(const char *content, long src_len);
        bool is_at_end() const;
        std::vector<Token> scan_tokens();
        void scan_token();


};


#endif // SCANNER_H_
