CC = g++
STD = -std=c++2a
LEXPARSE = lox.cpp LexParse/tokens.cpp LexParse/scanner.cpp LexParse/expr.cpp LexParse/parser.cpp LexParse/stmt.cpp 
INTERPRET = Interpreter/interpreter.cpp Interpreter/lox_function.cpp
ASAN = -fsanitize=address

all: build 

safe:
	$(CC) $(STD) main.cpp $(LEXPARSE) $(INTERPRET) $(ASAN) -g -o ./target/jlox

release:
	$(CC) $(STD) main.cpp $(LEXPARSE) $(INTERPRET) -O3 -g -o ./target/jlox

restart: 
	make clean && make build

build:
	$(CC) $(STD) main.cpp $(LEXPARSE) $(INTERPRET) -g -o ./target/jlox 

clean:
	rm ./target/jlox

.PHONY: clean
