#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Interpreter/interpreter.hpp"
#include "LexParse/scanner.hpp"
#include "LexParse/tokens.hpp"
#include "LexParse/expr.hpp"
#include "LexParse/parser.hpp"
#include "util.hpp"
#include "lox.hpp"

#define WRONG_USAGE (64)


static void print_usage() { printf("Usage: jlox [script]\n"); }

static void run_prompt() {
  // TODO: uninteresting for now
}

static void run(char *content, long content_len) {
  auto sc = Scanner(content, content_len);
  auto tokens = sc.scan_tokens();
  auto parser = Parser(std::move(tokens));
  auto e = parser.parse();
  auto interp = Interpreter{};
  try {
    LoxElement ret = interp.evaluate(e);
    std::cout << ret.as_number() << std::endl;
  } catch (LoxRuntimeErr rer) {
    std::cout << "Caught runtime error from interpreter\n";
  }
  // std::cout << e.parenthesize() << std::endl;
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

int main(int argc, char **argv) {
  Lox compiler;

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
