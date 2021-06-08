all: build

build:
	g++ main.cpp -g -o jlox && mkdir target && mv ./jlox ./target/jlox

clean:
	rm -rf ./target

.PHONY: clean
