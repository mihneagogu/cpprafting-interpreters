#ifndef LOX_H_
#define LOX_H_

#include <string>
#include <unordered_map>

#include "LexParse/tokens.hpp"

class Lox {
    private:
        static bool has_err;

    public:
        static std::unordered_map<std::string, TokenType> keywords;
        static void error(int line, std::string &message);
        static void error(int line, const char *message);
        static void error(Token &tok, const std::string &message);
        static void report(int line, const std::string &where, const std::string &message);
        static void report(int line, const std::string &where, const char *message);
        static bool hasError();
};



#endif // LOX_H_
