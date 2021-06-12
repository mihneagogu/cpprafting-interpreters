#ifndef MAIN_H_
#define MAIN_H_
#include <string>
#include <unordered_map>

#include "LexParse/tokens.hpp"

class Lox {
    private:
        static bool hasErr;

    public:
        static std::unordered_map<std::string, TokenType> keywords;
        static void error(int line, std::string &message);
        static void error(int line, const char *message);
        static void report(int line, std::string &where, std::string &message);
        static void report(int line, std::string &where, const char *message);
        static bool hasError();
};


#endif // MAIN_H_
