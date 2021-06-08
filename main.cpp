#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "LexParse/scanner.hpp"
#include "util.hpp"
#include "main.hpp"

#define WRONG_USAGE (64)

bool had_err = false;

// -- class Lox implementation //

bool Lox::hasErr = false;
void Lox::error(int line, std::string &message) {
    std::string empty = "";
    Lox::report(line, empty, message);
}

void Lox::report(int line, std::string &where, std::string &message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
    Lox::hasErr = true;
}

bool Lox::hasError() {
    return Lox::hasErr;
}

// -- class Lox implementation //

static void print_usage() {
    printf("Usage: jlox [script]\n");
}

static void run_prompt() {
    // TODO: uninteresting for now
}

static void run(char *content, long content_len) {
    auto sc = Scanner(content, content_len);
    auto tokens = sc.scan_tokens();
    free(content);
}



static void run_file(const char *file) {
    FILE *prog = fopen(file, "r");
    if (!prog) {
        printf("Could not open %s\n", file);
        exit(WRONG_USAGE);
    }
    fseek(prog, 0, SEEK_END);
    long len = ftell(prog);
    rewind(prog);
    char *content = (char *) malloc(sizeof(char) * len);
    ASSERT_ALLOC(content);
    size_t read = fread(content, len, 1, prog);
    if (read == 0) {
        printf("Something went wrong with reading the file. Exiting\n");
        free(content);
        exit(EXIT_FAILURE);
    }
    fclose(prog);
    run(content, len);
}

int main(int argc, char** argv) {
    Lox compiler;

    std::string msg = "Really long string that cannot use small string optimisation so we allocate it";
    compiler.error(0, msg);

    exit(EXIT_SUCCESS);
    if (argc > 2) {
        print_usage();
        exit(WRONG_USAGE);
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        run_prompt();
    }
    return 0;
}
