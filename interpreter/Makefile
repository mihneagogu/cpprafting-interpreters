CC = g++
STD = -std=c++2a
LEXPARSE = lox.cpp LexParse/tokens.cpp LexParse/scanner.cpp LexParse/expr.cpp LexParse/parser.cpp Interpreter/interpreter.cpp
ASAN = -fsanitize=address

all: restart

safe:
	make clean ; $(CC) $(STD) main.cpp $(LEXPARSE) $(ASAN) -g -o jlox && mkdir target && mv ./jlox ./target/jlox 

release:
	$(CC) $(STD) main.cpp $(LEXPARSE) -O3 -g -o jlox && mkdir target && mv ./jlox ./target/jlox

restart: 
	make clean && make build

build:
	$(CC) $(STD) main.cpp $(LEXPARSE) -g -o jlox && mkdir target && mv ./jlox ./target/jlox

clean:
	rm -rf ./target

.PHONY: clean
