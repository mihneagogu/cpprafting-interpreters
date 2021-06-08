#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "LexParse/scanner.cpp"

#define WRONG_USAGE (64)

bool had_err = false;

static void print_usage() {
    printf("Usage: jlox [script]\n");
}

static void run_prompt() {
    // TODO: uninteresting for now
}

static void run(char *content) {
    auto sc = Scanner(content);
    auto tokens = sc.scan_tokens();
    free(content);
}

static void report(int line, std::string where, std::string message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message << std::endl;
    had_err = true;
}

void error(int line, std::string message) {
    report(line, "", std::move(message));
    std::cout << "message after move: " << message << std::endl;
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
    size_t read = fread(content, len, 1, prog);
    if (read == 0) {
        printf("Something went wrong with reading the file. Exiting\n");
        free(content);
        exit(EXIT_FAILURE);
    }
    fclose(prog);
    run(content);
}

int main(int argc, char** argv) {
    std::string msg = "Really long string that cannot use small string optimisation so we allocate it";
    error(0, std::move(msg));

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
