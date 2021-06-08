#ifndef MAIN_H_
#define MAIN_H_
#include <string>

class Lox {
    private:
        static bool hasErr;

    public:
        static void error(int line, std::string &message);
        static void report(int line, std::string &where, std::string &message);
        static bool hasError();
};


#endif // MAIN_H_
