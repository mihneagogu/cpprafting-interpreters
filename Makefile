CC = g++
STD = -std=c++17
all: build

restart: 
	make clean && make

build:
	$(CC) $(STD) main.cpp LexParse/tokens.cpp LexParse/scanner.cpp -g -o jlox && mkdir target && mv ./jlox ./target/jlox

clean:
	rm -rf ./target

.PHONY: clean
