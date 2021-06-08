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

    public:
        Scanner(const char *content, long src_len);
        bool is_at_end() const;
        std::vector<Token> scan_tokens();
        void scan_token();

};


#endif // SCANNER_H_
